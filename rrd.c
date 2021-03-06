/*
 * rrd.c
 *
 * PHP interface to RRD Tool. (for php/zend)
 *
 *
 * Joe Miller, <joeym@joeym.net> 2/12/2000 & 7/18/2000 php4-rrdtool 1.04
 * JoungKyun.Kim, <http://devel.oops.org> forking mod_rrd
 *
 * Copyright (c) 2007, JoungKyun.Kim
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of the Joseph Miller nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * See README, INSTALL, and USAGE files for more details.
 *
 * $Id$
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"
#include "ext/standard/info.h"

#include "rrd.h"
#include "php_rrd.h"

#if HAVE_RRD

/* True global resources - no need for thread safety here */
static int le_rrd;

/* {{{ rrd_functions[]
 *
 * Every user visible function must have an entry in rrd_functions[].
 * todo: rrdxport
 */
const zend_function_entry rrd_functions[] = {
	PHP_FE(confirm_rrd_compiled, NULL)
	PHP_FE(rrd_error, NULL)
	PHP_FE(rrd_clear_error, NULL)
	PHP_FE(rrd_graph, NULL)
	PHP_FE(rrd_last, NULL)
	PHP_FE(rrd_fetch, NULL)
	PHP_FE(rrd_update, NULL)
	PHP_FE(rrd_create, NULL)
	PHP_FE(rrd_dump, NULL)
	PHP_FE(rrd_restore, NULL)
	PHP_FE(rrd_first, NULL)
	PHP_FE(rrd_info, NULL)
	PHP_FE(rrd_resize, NULL)
	PHP_FE(rrd_lastupdate, NULL)
	PHP_FE(rrd_tune, NULL)
	PHP_FE(rrd_xport, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ rrd_module_entry
 */
zend_module_entry rrd_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"rrd",
	rrd_functions,
	PHP_MINIT(rrd),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(rrd),
#if ZEND_MODULE_API_NO >= 20010901
	RRD_BUILDVER,
#endif
	STANDARD_MODULE_PROPERTIES,
};
/* }}} */

#ifdef COMPILE_DL_RRD
ZEND_GET_MODULE(rrd)
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(rrd)
{
	REGISTER_LONG_CONSTANT ("RRD_RANGE_CHECK", RRD_RANGE_CHECK, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT ("RRD_OVER_WRITE", RRD_OVER_WRITE, CONST_PERSISTENT | CONST_CS);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(rrd)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "rrdtool support", "enabled");
	php_info_print_table_row(2, "Build version", RRD_BUILDVER);
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto (string) rrd_error (void)
 * Get the error message set by the last rrd tool function call
 */
PHP_FUNCTION(rrd_error) {
	char * msg;

	if ( rrd_test_error () ) {
		msg = rrd_get_error ();        

		RETVAL_STRING (msg, 1);
		rrd_clear_error ();
	}
}
/* }}} */

/* {{{ proto (void) rrd_clear_error (void)
 * Clear the error set by the last rrd tool function call
 */
PHP_FUNCTION(rrd_clear_error) {
	if ( rrd_test_error () )
		rrd_clear_error ();
}
/* }}} */

/* {{{ proto (bool) rrd_update (string file, string opt)
 * Update an RRD file with values specified
 */
PHP_FUNCTION(rrd_update) {
	char  * file = NULL,
		  * opt  = NULL;
	int     flen = 0,
		    olen = 0;

	char ** argv;

	if ( rrd_test_error () )
		rrd_clear_error ();

	if ( rrd_parameters ("ss", &file, &flen, &opt, &olen) == FAILURE )
		return;

	if ( flen == 0 ) {
		php_error (E_WARNING, "1st argument is empty or missing\n");
		RETURN_FALSE;
	}

	argv = (char **) emalloc (4 * sizeof (char *));

	argv[0] = "dummy";
	argv[1] = estrdup ("update");
	argv[2] = estrdup (file);
	argv[3] = olen ? estrdup (opt) : NULL;

	optind = 0;
	opterr = 0;

	if ( rrd_update (3, &argv[1]) != -1 )
		RETVAL_TRUE;
	else
		RETVAL_FALSE;

	safe_efree (argv[1]);
	safe_efree (argv[2]);
	safe_efree (argv[3]);
	safe_efree (argv);
}
/* }}} */

/* {{{ proto (int|false) rrd_last (string file)
 * Gets last update time of an RRD file
 */
PHP_FUNCTION(rrd_last) {
	char        * file = NULL;
	int           flen = 0;
	unsigned long retval;
	char       ** argv;

	if ( rrd_test_error () )
		rrd_clear_error ();

	if ( rrd_parameters ("s", &file, &flen) == FAILURE )
		return;

	if ( flen == 0 ) {
		php_error (E_WARNING, "1st argument is empty or missing\n");
		RETURN_FALSE;
	}

	argv = (char **) emalloc(3 * sizeof(char *));

	argv[0] = "dummy";
	argv[1] = estrdup ("last");
	argv[2] = flen ? estrdup (file) : NULL;

	optind = 0;
	opterr = 0;
	retval = rrd_last (2, &argv[1]);

	safe_efree (argv[1]);
	safe_efree (argv[2]);
	safe_efree (argv);

	RETVAL_LONG(retval);
}
/* }}} */

/* {{{ proto (bool) rrd_create (string file, array args_arr, int argc)
 * Create an RRD file with the options passed (passed via array)
 */ 
PHP_FUNCTION(rrd_create) {
	char      * file   = NULL;
	zval      * args   = NULL;
	int         p_argc = 0,
				flen   = 0,
				argc   = 0,
				i;
	HashTable * args_arr = NULL;
	HashPosition pos;
	char     ** argv     = NULL;

	if ( rrd_test_error () )
		rrd_clear_error ();

	if ( rrd_parameters ("szl", &file, &flen, &args, &p_argc) == FAILURE )
		return;

	if ( flen == 0 ) {
		php_error (E_WARNING, "1st argument is empty or missing\n");
		RETURN_FALSE;
	}

	args_arr = Z_ARRVAL_P (args);
	zend_hash_internal_pointer_reset_ex (args_arr, &pos);

	argc = p_argc + 3;
	argv = (char **) emalloc (argc * sizeof (char *));

	argv[0] = "dummy";
	argv[1] = estrdup ("create");
	argv[2] = estrdup (file);

	for ( i = 3; i < argc; i++ ) {
		zval ** dataptr;

		if ( zend_hash_get_current_data_ex (args_arr, (void **) &dataptr, &pos) == FAILURE )
			continue;

		if ( Z_TYPE_PP (dataptr) != IS_STRING )
			convert_to_string_ex (dataptr);

		argv[i] = estrdup (Z_STRVAL_PP (dataptr));

		if ( i < argc )
			zend_hash_move_forward_ex (args_arr, &pos);
	}

	optind = 0;
	opterr = 0;

	if ( rrd_create (--argc, &argv[1]) != -1 )
		RETVAL_TRUE;
	else
		RETVAL_FALSE;

	for ( i=1; i<argc; i++ )
		safe_efree (argv[i]);

	safe_efree (argv);
}
/* }}} */

/* {{{ proto (array|false) rrd_graph (string file, array args_arr, int argc)
 * Creates a graph based on options passed via an array
 */
PHP_FUNCTION(rrd_graph) {
	char         * file   = NULL;
	zval         * args;
	int            p_argc = 0,
				   flen   = 0;

	zval         * p_calcpr;
	HashTable    * args_arr;
	HashPosition   pos;
	int            i, xsize, ysize, argc;
	char        ** argv;
	char        ** calcpr;
	double         ymin, ymax;

	if ( rrd_test_error () )
		rrd_clear_error ();

	if ( rrd_parameters ("szl", &file, &flen, &args, &p_argc) == FAILURE )
		return;

	if ( ! flen ) {
		php_error (E_WARNING, "1st argumnet is empty or missing!");
		RETURN_FALSE;
	}

	args_arr = Z_ARRVAL_P (args);
	zend_hash_internal_pointer_reset_ex (args_arr, &pos);

	argc = p_argc + 3;
	argv = (char **) emalloc(argc * sizeof(char *));

	argv[0] = "dummy";
	argv[1] = estrdup("graph");
	argv[2] = estrdup(file);

	for ( i=3; i<argc; i++ ) {
		zval **dataptr;

		if ( zend_hash_get_current_data_ex (args_arr, (void *) &dataptr, &pos) == FAILURE )
			continue;

		if ( Z_TYPE_PP (dataptr) != IS_STRING )
			convert_to_string_ex (dataptr);

		argv[i] = estrdup(Z_STRVAL_PP (dataptr));

		if ( i < argc )
			zend_hash_move_forward_ex (args_arr, &pos);
	}

	optind = 0;
	opterr = 0; 

	if ( rrd_graph(argc-1, &argv[1], &calcpr, &xsize, &ysize, NULL, &ymin, &ymax) == -1 )
		RETURN_FALSE;

	array_init (return_value);
	add_assoc_long (return_value, "xsize", xsize);
	add_assoc_long (return_value, "ysize", ysize);
	add_assoc_long (return_value, "ymin", ymin);
	add_assoc_long (return_value, "ymax", ymax);

	MAKE_STD_ZVAL (p_calcpr);
	array_init (p_calcpr);

	if ( calcpr ) {
		for ( i=0; calcpr[i]; i++ ) {
			add_next_index_string (p_calcpr, calcpr[i], 1);
			free (calcpr[i]);
		}
		free (calcpr);
	}
	zend_hash_update (
		return_value->value.ht, "calcpr", sizeof("calcpr"), 
		(void *)&p_calcpr, sizeof(zval *), NULL
	);

	for ( i=1; i<argc; i++ )
		safe_efree (argv[i]);

	safe_efree(argv);
}
/* }}} */

/* {{{ proto (array|false) rrd_fetch (string file, array args_arr, int p_argc)
 * Fetch info from an RRD file
 */
PHP_FUNCTION(rrd_fetch) {
	char           * file = NULL;
	zval           * args;
	int              p_argc = 0,
					 flen   = 0;
	
	char          ** argv,
				  ** ds_namv;
	time_t           start, end;
	unsigned long    step, ds_cnt;
	rrd_value_t    * data,
				   * datap;
	int              argc, i, x;

	HashTable      * args_arr;
	HashPosition     pos;
	zval           * p_ds_namv,
				   * p_data,
				   * p_sum;

	if ( rrd_test_error () )
		rrd_clear_error ();

	if ( rrd_parameters ("szl", &file, &flen, &args, &p_argc) == FAILURE )
		return;

	if ( ! flen ) {
		php_error (E_WARNING, "1st argumnet is empty or missing!");
		RETURN_FALSE;
	}

	args_arr = Z_ARRVAL_P (args);
	zend_hash_internal_pointer_reset_ex (args_arr, &pos);

	argc = p_argc + 3;
	argv = (char **) emalloc (argc * sizeof (char *));

	argv[0] = "dummy";
	argv[1] = estrdup("fetch");
	argv[2] = estrdup(file);

	for ( i=3; i<argc; i++) {
		zval **dataptr;

		if ( zend_hash_get_current_data_ex (args_arr, (void **) &dataptr, &pos) == FAILURE )
			continue;

		if ( Z_TYPE_PP (dataptr) != IS_STRING )
			convert_to_string_ex (dataptr);

		argv[i] = estrdup (Z_STRVAL_PP (dataptr));

		if ( i < argc )
			zend_hash_move_forward_ex (args_arr, &pos);
	}

	optind = 0;
	opterr = 0; 

	if ( rrd_fetch (argc -1, &argv[1], &start, &end, &step, &ds_cnt, &ds_namv, &data) == -1 )
		RETURN_FALSE;

	if ( array_init (return_value) == FAILURE ) {
		RETVAL_FALSE;
		goto go_free;
	}

	add_assoc_long (return_value, "start", start);
	add_assoc_long (return_value, "end", end);
	add_assoc_long (return_value, "step", step);
	add_assoc_long (return_value, "ds_cnt", ds_cnt);

	MAKE_STD_ZVAL (p_ds_namv);
	MAKE_STD_ZVAL (p_data);
	MAKE_STD_ZVAL (p_sum);
	array_init (p_ds_namv);
	array_init (p_data);
	array_init (p_sum);

	if ( ds_namv ) {
		for ( i=0; i<ds_cnt; i++ ) {
			add_next_index_string (p_ds_namv, ds_namv[i], 1);
			free (ds_namv[i]);
		}
		free (ds_namv);
	}

	if ( data ) {
		datap = data;

		for ( i=start; i<=end; i += step ) {
			char t_time[32]    = { 0, },
				 t_value[1024] = { 0, },
				 t_sum[2048]   = { 0, };
			int  tlen = 0;

			sprintf (t_value, "%10lu: ", i);
			tlen = strlen (t_value);

			for ( x=0; x<ds_cnt; x++ ) {
				double tmp;
				char   tmp_value[128] = { 0, };

				tmp = *(datap++);
				add_next_index_double (p_data, tmp);

				sprintf (tmp_value, " %0.10e", tmp);
				memcpy (t_value + tlen, tmp_value, strlen (tmp_value));
				tlen = strlen (t_value);
			}
			add_next_index_string (p_sum, t_value, 1);
		}

		free(data);
	}

	zend_hash_update (
		return_value->value.ht, "ds_namv", sizeof("ds_namv"), 
		(void *)&p_ds_namv, sizeof(zval *), NULL
	);
	zend_hash_update (
		return_value->value.ht, "data", sizeof("data"), 
		(void *)&p_data, sizeof(zval *), NULL
	);
	zend_hash_update (
		return_value->value.ht, "sum", sizeof("sum"), 
		(void *)&p_sum, sizeof(zval *), NULL
	);

go_free:
	for ( i=1; i<argc; i++ )
		safe_efree (argv[i]);

	safe_efree (argv);
}
/* }}} */

/* {{{ proto (bool) rrd_dump (string file) 
 * Dump an RRD file with XML format
 */
PHP_FUNCTION(rrd_dump) {
	char ** argv,
		  * file = NULL,
		  * temp = NULL;
	int     flen = 0,
			nohead = 0,
			argc = ZEND_NUM_ARGS (),
			i, n;
	zval  * ob;

	FILE * fp;
	char buf[8192] = { 0, };

	if ( rrd_test_error() )
		rrd_clear_error();

	// nohead is 1.3
	if ( rrd_parameters ("s|b", &file, &flen, &nohead) == FAILURE )
		return;

	if ( ! flen ) {
		php_error (E_WARNING, "1st argumnet is empty or missing!");
		RETURN_FALSE;
	}

	argv = (char **) emalloc (5 * sizeof (char *));

	if ( (temp = tempnam ("/tmp", "rre-")) == NULL ) {
		php_error (E_WARNING, "Failed create temporary file");
		RETURN_FALSE;
	}

	n = 0;
	argv[n] = "dummy";
	argv[++n] = estrdup ("dump");
#ifdef SUPPORT_RRD13
	if ( nohead )
		argv[++n] = estrdup ("-n");
#endif
	argv[++n] = estrdup (file);
	argv[++n] = estrdup (temp);

	optind = 0;
	opterr = 0;

	flen = rrd_dump (n, &argv[1]);

	if ( flen != 0 ) {
		RETVAL_FALSE;
		goto free_var;
	}

	if ( (fp = fopen (temp, "r")) == NULL ) {
		unlink (temp);
		RETVAL_FALSE;
		goto free_var;
	}

	while ( (i = fread (&buf, sizeof (char), 8000, fp)) ) {
		PHPWRITE (buf, i);
		memset (buf, 0, 8192);
	}

	fclose (fp);
	unlink (temp);

	RETVAL_TRUE;

free_var:
	for ( i=1; i<=n; i++ )
		safe_efree (argv[i]);
	safe_efree (argv);
}
/* }}} */

/* {{{ proto (bool) rrd_restore (string src, string dst, int opts) 
 * Restore an RRD file from XML format
 */
PHP_FUNCTION(rrd_restore) {
	char  ** argv,
		   * src = NULL,
		   * dst = NULL;
	int      slen = 0,
			 dlen = 0,
			 opt  = 0,
			 opts = 0,
			 argc = ZEND_NUM_ARGS (),
			 i;

	if ( rrd_test_error () )
		rrd_clear_error ();

	if ( rrd_parameters ("ss|l", &src, &slen, &dst, &dlen, &opt) == FAILURE )
		return;

	if ( ! slen ) {
		php_error (E_WARNING, "1st argumnet is empty or missing!");
		RETURN_FALSE;
	}

	if ( ! dlen ) {
		php_error (E_WARNING, "2st argumnet is empty or missing!");
		RETURN_FALSE;
	}

	if ( opt ) {
		if ( opt == 0x03 )
			argc++;
		else if ( opt > 0x03 ) {
			php_error (E_WARNING, "Invalid rrd restore options");
			RETURN_FALSE;
		}
	} else if ( ! opt && argc == 3 )
		argc--;

	opts = argc + 2;

	argv = (char **) emalloc (opts * sizeof (char *));

	i = 0;
	argv[i] = "dummy";
	argv[++i] = estrdup ("restore");
	argc = 1;

	switch ( opt ) {
		case 0x01 :
			argv[++i] = estrdup ("-r");
			argc++;
			break;
		case 0x02 : 
			argv[++i] = estrdup ("-f");
			argc++;
			break;
		case 0x03 :
			argv[++i] = estrdup ("-r");
			argv[++i] = estrdup ("-f");
			argc += 2;
			break;
	}

	argv[++i] = estrdup (src);
	argv[++i] = estrdup (dst);
	argc += 2;

	optind = 0;
	opterr = 0;

	if ( rrd_restore (argc, &argv[1]) != -1 )
		RETVAL_TRUE;
	else
		RETVAL_FALSE;

	for ( i=1; i<opts; i++ )
		safe_efree (argv[i]);
	safe_efree (argv);
}
/* }}} */

/* {{{ proto (int|false) rrd_first (string file, string index)
 * Return the date of the first data sample in an RRA within an RRD
 */
PHP_FUNCTION(rrd_first) {
	char         * file   = NULL,
				 * index  = NULL;
	int             flen   = 0,
					ilen   = 0,
					f_argc = 2,
					i;

	char         ** argv;
	unsigned int    retval = 0;

	if ( rrd_test_error () )
		rrd_clear_error ();

	if ( rrd_parameters ("s|s", &file, &flen, &index, &ilen) == FAILURE )
		return;

	if ( flen == 0 ) {
		php_error (E_WARNING, "1st argument is empty or missing\n");
		RETURN_FALSE;
	}

	argv = (char **) emalloc (4 * sizeof (char *));

	argv[0] = estrdup ("dummy");
	argv[1] = estrdup ("first");
	argv[2] = estrdup (file);
	if ( ilen ) {
		argv[3] = estrdup (index);
		f_argc++;
	}

	optind = 0;
	opterr = 0;
	retval = rrd_first (f_argc, &argv[1]);

	for ( i=0; i<=f_argc; i++ )
		safe_efree (argv[i]);
	safe_efree (argv);
	RETVAL_LONG (retval);
}
/* }}} */

/* {{{ proto (bool) rrd_info (string file)
 */
PHP_FUNCTION(rrd_info) {
	char       * file   = NULL;
	int          flen   = 0, i;

	char       * argv[2];
	rrd_info_t * data;

	if ( rrd_test_error () )
		rrd_clear_error ();

	if ( rrd_parameters ("s", &file, &flen) == FAILURE )
		return;

	if ( flen == 0 ) {
		php_error (E_WARNING, "1st argument is empty or missing\n");
		RETURN_FALSE;
	}

	argv[0] = "info";
	argv[1] = file;

	optind = 0;
	opterr = 0;
	data = rrd_info (2, argv);

	while ( data ) {
		php_printf ("%s = ", data->key);

		switch (data->type) {
			case RD_I_VAL:
				if (isnan(data->value.u_val))
					php_printf("NaN\n");
				else
					php_printf("%0.10e\n", data->value.u_val);
				break;
			case RD_I_CNT:
				php_printf("%lu\n", data->value.u_cnt);
				break;
			case RD_I_INT:
				php_printf("%d\n", data->value.u_int);
				break;
			case RD_I_STR:
				php_printf("\"%s\"\n", data->value.u_str);
				break;
			case RD_I_BLO:
				php_printf("BLOB_SIZE:%lu\n", data->value.u_blo.size);
				//fwrite(data->value.u_blo.ptr, data->value.u_blo.size, 1, stdout);
				PHPWRITE (data->value.u_blo.ptr, data->value.u_blo.size);
				break;

		}
		data = data->next;
	}

	rrd_info_free (data);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto (bool) rrd_resize (string rrdfile, array args_arr, int p_argc)
 * Fetch info from an RRD file
 */
PHP_FUNCTION(rrd_resize) {
	char           * file = NULL;
	zval           * args;
	int              p_argc = 0,
					 flen   = 0;
	
	char           * argv[6],
				  ** ds_namv;
	time_t           start, end;
	unsigned long    step, ds_cnt;
	rrd_value_t    * data,
				   * datap;
	int              argc, i, x;

	HashTable      * args_arr;
	HashPosition     pos;
	zval           * p_ds_namv,
				   * p_data,
				   * p_sum;

	if ( rrd_test_error () )
		rrd_clear_error ();

	if ( rrd_parameters ("szl", &file, &flen, &args, &p_argc) == FAILURE )
		return;

	if ( ! flen ) {
		php_error (E_WARNING, "1st argumnet is empty or missing!");
		RETURN_FALSE;
	}

	args_arr = Z_ARRVAL_P (args);
	zend_hash_internal_pointer_reset_ex (args_arr, &pos);

	argc = 6;

	argv[0] = "rrdtool";
	argv[1] = estrdup("resize");
	argv[2] = estrdup(file);

	for ( i=3; i<argc; i++) {
		zval **dataptr;

		if ( zend_hash_get_current_data_ex (args_arr, (void **) &dataptr, &pos) == FAILURE )
			continue;

		if ( Z_TYPE_PP (dataptr) != IS_STRING )
			convert_to_string_ex (dataptr);

		argv[i] = estrdup (Z_STRVAL_PP (dataptr));

		if ( i < argc )
			zend_hash_move_forward_ex (args_arr, &pos);
	}

	optind = 0;
	opterr = 0; 

	if ( rrd_resize (5, &argv[1]) == -1 )
		RETURN_FALSE;

	for ( i=1; i<argc; i++ )
		safe_efree (argv[i]);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto (bool) rrd_lastupdate (string file)
 */
PHP_FUNCTION(rrd_lastupdate) {
	char       * file   = NULL;
	int          flen   = 0, i;

	char       * argv[2];
	rrd_info_t * data;

	if ( rrd_test_error () )
		rrd_clear_error ();

	if ( rrd_parameters ("s", &file, &flen) == FAILURE )
		return;

	if ( flen == 0 ) {
		php_error (E_WARNING, "1st argument is empty or missing\n");
		RETURN_FALSE;
	}

	argv[0] = "lastupdate";
	argv[1] = file;

	optind = 0;
	opterr = 0;

	{
		time_t           last_update;
		char          ** ds_namv;
		char          ** last_ds;
		unsigned long    ds_cnt, i;

		if ( rrd_lastupdate (2, argv, &last_update, &ds_cnt, &ds_namv, &last_ds) != 0 )
			RETURN_FALSE;

		array_init (return_value);
		add_assoc_long (return_value, "stamp", last_update);

		for ( i=0; i<ds_cnt; i++ ) {
			add_assoc_string (return_value, ds_namv[i], last_ds[i], 1);
			free (last_ds[i]);
			free (ds_namv[i]);
		}
		free (last_ds);
		free (ds_namv);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto (bool) rrd_tune (string rrdfile, array args_arr, int p_argc)
 */
PHP_FUNCTION(rrd_tune) {
	char           * file = NULL;
	zval           * args;
	int              p_argc = 0,
					 flen   = 0;
	
	char          ** argv;
	rrd_value_t    * data,
				   * datap;
	int              argc, i;

	HashTable      * args_arr;
	HashPosition     pos;

	if ( rrd_test_error () )
		rrd_clear_error ();

	if ( rrd_parameters ("szl", &file, &flen, &args, &p_argc) == FAILURE )
		return;

	if ( ! flen ) {
		php_error (E_WARNING, "1st argumnet is empty or missing!");
		RETURN_FALSE;
	}

	args_arr = Z_ARRVAL_P (args);
	zend_hash_internal_pointer_reset_ex (args_arr, &pos);

	argc = p_argc + 3;

	argv = (char **) emalloc (sizeof (char *) * argc);

	argv[0] = "rrdtool";
	argv[1] = estrdup("resize");
	argv[2] = estrdup(file);

	for ( i=3; i<argc; i++) {
		zval **dataptr;

		if ( zend_hash_get_current_data_ex (args_arr, (void **) &dataptr, &pos) == FAILURE )
			continue;

		if ( Z_TYPE_PP (dataptr) != IS_STRING )
			convert_to_string_ex (dataptr);

		argv[i] = estrdup (Z_STRVAL_PP (dataptr));

		if ( i < argc )
			zend_hash_move_forward_ex (args_arr, &pos);
	}

	optind = 0;
	opterr = 0; 

	if ( rrd_tune (argc - 1, &argv[1]) == -1 )
		RETURN_FALSE;

	for ( i=1; i<argc; i++ )
		safe_efree (argv[i]);
	safe_efree (argv);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto (bool) rrd_xport (string rrdfile, array args_arr, int p_argc)
 */
PHP_FUNCTION(rrd_xport) {
	zval           * args;
	int              p_argc = 0;
	
	char          ** argv;
	rrd_value_t    * data,
				   * datap;
	int              argc, i,
				     enumds = 0;

	HashTable      * args_arr;
	HashPosition     pos;

	if ( rrd_test_error () )
		rrd_clear_error ();

	if ( rrd_parameters ("zl", &args, &p_argc) == FAILURE )
		return;

	args_arr = Z_ARRVAL_P (args);
	zend_hash_internal_pointer_reset_ex (args_arr, &pos);

	argc = p_argc + 2;

	argv = (char **) emalloc (sizeof (char *) * argc);

	argv[0] = "rrdtool";
	argv[1] = estrdup("resize");

	for ( i=2; i<argc; i++) {
		zval **dataptr;

		if ( zend_hash_get_current_data_ex (args_arr, (void **) &dataptr, &pos) == FAILURE )
			continue;

		if ( Z_TYPE_PP (dataptr) != IS_STRING )
			convert_to_string_ex (dataptr);

		argv[i] = estrdup (Z_STRVAL_PP (dataptr));

		if ( strcmp ("--enumds", argv[i]) == 0 )
			enumds = 1;

		if ( i < argc )
			zend_hash_move_forward_ex (args_arr, &pos);
	}

	optind = 0;
	opterr = 0; 

	{
		int                  xxsize;
		unsigned long int    j = 0;
		time_t               start, end, ti;
		unsigned long        step, col_cnt, row_cnt;
		rrd_value_t       *  data, * ptr;
		char              ** legend_v;
		size_t               vtag_s = strlen (COL_DATA_TAG) + 10;
		char               * vtag = malloc (vtag_s);


		if ( rrd_xport (argc - 1, &argv[1], &xxsize, &start, &end, &step, &col_cnt, &legend_v, &data) == -1 )
			RETURN_FALSE;

		row_cnt = (end - start) / step;
		ptr = data;
		php_printf ("<?xml version=\"1.0\" encoding=\"%s\"?>\n\n", XML_ENCODING);
		php_printf ("<%s>\n", ROOT_TAG);
		php_printf ("  <%s>\n", META_TAG);
		php_printf ("    <%s>%lu</%s>\n", META_START_TAG,
				(unsigned long) start + step, META_START_TAG);
		php_printf ("    <%s>%lu</%s>\n", META_STEP_TAG, step, META_STEP_TAG);
		php_printf ("    <%s>%lu</%s>\n", META_END_TAG, (unsigned long) end, META_END_TAG);
		php_printf ("    <%s>%lu</%s>\n", META_ROWS_TAG, row_cnt, META_ROWS_TAG);
		php_printf ("    <%s>%lu</%s>\n", META_COLS_TAG, col_cnt, META_COLS_TAG);
		php_printf ("    <%s>\n", LEGEND_TAG);
		for ( j=0; j<col_cnt; j++ ) {
			char * entry = NULL;

			entry = legend_v[j];
			php_printf ("      <%s>%s</%s>\n", LEGEND_ENTRY_TAG, entry, LEGEND_ENTRY_TAG);
			free (entry);
		}
		free (legend_v);
		php_printf ("    </%s>\n", LEGEND_TAG);
		php_printf ("  </%s>\n", META_TAG);
		php_printf ("  <%s>\n", DATA_TAG);
		for ( ti=start + step; ti<=end; ti += step ) {
			php_printf ("    <%s>", DATA_ROW_TAG);
			php_printf ("<%s>%lu</%s>", COL_TIME_TAG, ti, COL_TIME_TAG);
			for ( j=0; j<col_cnt; j++ ) {
				rrd_value_t newval = DNAN;

				if ( enumds == 1 )
					snprintf (vtag, vtag_s, "%s%lu", COL_DATA_TAG, j);
				else
					snprintf (vtag, vtag_s, "%s", COL_DATA_TAG);
				newval = * ptr;
				if ( isnan (newval) ) {
					php_printf ("<%s>NaN</%s>", vtag, vtag);
				} else {
					php_printf ("<%s>%0.10e</%s>", vtag, newval, vtag);
				};
				ptr++;
			}
			php_printf ("</%s>\n", DATA_ROW_TAG);
		}
		free (data);
		php_printf ("  </%s>\n", DATA_TAG);
		php_printf ("</%s>\n", ROOT_TAG);
		free (vtag);
	}

	for ( i=1; i<argc; i++ )
		safe_efree (argv[i]);
	safe_efree (argv);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Every user-visible function in PHP should document itself in the source
 * proto string confirm_rrd_compiled(string arg)
 * Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_rrd_compiled)
{
	char * arg = NULL;
	int    alen, len;
	char   string[256] = { 0, };

	if ( rrd_parameters ("s", &arg, &alen) == FAILURE )
		return;

	if ( alen == 0 ) {
		php_error (E_WARNING, "1st argument is empty or missing!");
		RETURN_FALSE;
	}

	len = sprintf (string, "Congratulations! You have successfully "
						  "modified ext/%.78s/config.m4. Module %.78s "
						  "is now compiled into PHP.", "rrd", arg);
	RETURN_STRINGL (string, len, 1);
}
/* }}} */

#endif	/* HAVE_RRD */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
