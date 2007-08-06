/*
 * rrd.c
 *
 * PHP interface to RRD Tool. (for php/zend)
 *
 *
 * Joe Miller, <joeym@joeym.net> 2/12/2000 & 7/18/2000 php4-rrdtool 1.04
 * JoungKyun Kim, <http://devel.oops.org> forking mod_rrd
 *
 * Copyright (c) 2004, JoungKyun Kim
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
 * $Id: rrd.c,v 1.2 2007-08-06 08:04:20 oops Exp $
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
 */
function_entry rrd_functions[] = {
	PHP_FE(confirm_rrdtool_compiled, NULL)
	PHP_FE(rrd_error, NULL)
	PHP_FE(rrd_clear_error, NULL)
	PHP_FE(rrd_graph, NULL)
	PHP_FE(rrd_last, NULL)
	PHP_FE(rrd_fetch, NULL)
	PHP_FE(rrd_update, NULL)
	PHP_FE(rrd_create, NULL)
	PHP_FE(rrd_dump, NULL)
	PHP_FE(rrd_restore, NULL)
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
	NULL,
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

/* {{{ proto string rrd_error(void)
 * Get the error message set by the last rrd tool function call
 */
PHP_FUNCTION(rrd_error) {
	char	* msg;

	if ( rrd_test_error() ) {
		msg = rrd_get_error();        

		RETVAL_STRING(msg, 1);
		rrd_clear_error();
	}
	else
		return;
}
/* }}} */

/* {{{ proto void rrd_clear_error(void)
 * Clear the error set by the last rrd tool function call
 */
PHP_FUNCTION(rrd_clear_error) {
	if ( rrd_test_error() )
		rrd_clear_error();

	return;
}
/* }}} */

/* {{{ proto int rrd_update(string file, string opt)
 * Update an RRD file with values specified
 */
PHP_FUNCTION(rrd_update) {
	pval	 * file;
	pval	 * opt;
	char	** argv;

	if ( rrd_test_error() )
		rrd_clear_error();

	if ( ZEND_NUM_ARGS() == 2 && zend_get_parameters(ht, 2, &file, &opt) == SUCCESS ) {
		convert_to_string(file);
		convert_to_string(opt);

		argv = (char **) emalloc(4 * sizeof(char *));

		argv[0] = "dummy";
		argv[1] = estrdup("update");
		argv[2] = estrdup(file->value.str.val);
		argv[3] = estrdup(opt->value.str.val);

		optind = 0; opterr = 0;
		if ( rrd_update(3, &argv[1]) != -1 ) {
			RETVAL_TRUE;
		} else {
			RETVAL_FALSE;
		}
		efree(argv[1]);
		efree(argv[2]);
		efree(argv[3]);
		efree(argv);
	} else {
		WRONG_PARAM_COUNT;
	}
	return;
}
/* }}} */

/* {{{ proto int rrd_last(string file)
 * Gets last update time of an RRD file
 */
PHP_FUNCTION(rrd_last) {
	pval			* file;
	unsigned long	  retval;

	char **argv = (char **) emalloc(3 * sizeof(char *));
    
	if ( rrd_test_error() )
		rrd_clear_error();
    
	if (zend_get_parameters(ht, 1, &file) == SUCCESS) {
		convert_to_string(file);

		argv[0] = "dummy";
		argv[1] = estrdup("last");
		argv[2] = estrdup(file->value.str.val);

		optind = 0; opterr = 0;
		retval = rrd_last(2, &argv[1]);

		efree(argv[1]);  efree(argv[2]);
		efree(argv);
		RETVAL_LONG(retval);
	} else {
		WRONG_PARAM_COUNT;
	}
	return;
}
/* }}} */

/* {{{ proto int rrd_create(string file, array args_arr, int argc)
 * Create an RRD file with the options passed (passed via array)
 */ 
PHP_FUNCTION(rrd_create) {
	pval		 * file;
	pval		 * args;
	pval		 * p_argc;
	pval		 * entry;
	char		** argv;
	HashTable	 * args_arr;
	int			   argc, i;

	if ( rrd_test_error() )
		rrd_clear_error();

	if ( ZEND_NUM_ARGS() == 3 && getParameters(ht, 3, &file, &args, &p_argc) == SUCCESS ) {
		if ( args->type != IS_ARRAY ) { 
			php_error(E_WARNING, "2nd Variable passed to rrd_create is not an array!\n");
			RETURN_FALSE;
		}

		convert_to_long(p_argc);
		convert_to_string(file);
		
		convert_to_array(args);
		args_arr = args->value.ht;
		zend_hash_internal_pointer_reset(args_arr);

		argc = p_argc->value.lval + 3;
		argv = (char **) emalloc(argc * sizeof(char *));

		argv[0] = "dummy";
		argv[1] = estrdup("create");
		argv[2] = estrdup(file->value.str.val);

		for (i = 3; i < argc; i++) {
			pval **dataptr;

			if ( zend_hash_get_current_data(args_arr, (void *) &dataptr) == FAILURE )
				continue;

			entry = *dataptr;

			if ( entry->type != IS_STRING )
				convert_to_string(entry);

			argv[i] = estrdup(entry->value.str.val);

			if ( i < argc )
				zend_hash_move_forward(args_arr);
		}
  
		optind = 0;
		opterr = 0;

		if ( rrd_create(argc-1, &argv[1]) != -1 ) {
			RETVAL_TRUE;
		} else {
			RETVAL_FALSE;
		}
		for (i = 1; i < argc; i++)
			efree(argv[i]);

		efree(argv);
	} else {
	    WRONG_PARAM_COUNT;
	}
	return;
}
/* }}} */

/* {{{ proto mixed rrd_graph(string file, array args_arr, int argc)
 * Creates a graph based on options passed via an array
 */
PHP_FUNCTION(rrd_graph) {
	pval		 * file;
	pval		 * args;
	pval		 * p_argc;
	pval		 * entry;
	zval		 * p_calcpr;
	HashTable	 * args_arr;
	int			   i, xsize, ysize, argc;
	char		** argv;
	char		** calcpr;
#ifdef SUPPORT_RRD12
	double		   ymin, ymax;
#endif
    

	if ( rrd_test_error() )
		rrd_clear_error();
    
	if ( ZEND_NUM_ARGS() == 3 && zend_get_parameters(ht, 3, &file, &args, &p_argc) == SUCCESS) {
		if ( args->type != IS_ARRAY ) { 
			php_error(E_WARNING, "2nd Variable passed to rrd_graph is not an array!\n");
			RETURN_FALSE;
		}
        
		convert_to_long(p_argc);
		convert_to_string(file);

		convert_to_array(args);
		args_arr = args->value.ht;

		argc = p_argc->value.lval + 3;
		argv = (char **) emalloc(argc * sizeof(char *));
 
		argv[0] = "dummy";
		argv[1] = estrdup("graph");
		argv[2] = estrdup(file->value.str.val);

		for (i = 3; i < argc; i++) {
			pval **dataptr;

			if ( zend_hash_get_current_data(args_arr, (void *) &dataptr) == FAILURE )
				continue;

			entry = *dataptr;

			if ( entry->type != IS_STRING )
				convert_to_string(entry);

			argv[i] = estrdup(entry->value.str.val);

			if ( i < argc )
				zend_hash_move_forward(args_arr);
		}
   
		optind = 0; opterr = 0; 
#ifdef SUPPORT_RRD12
		if ( rrd_graph(argc-1, &argv[1], &calcpr, &xsize, &ysize, NULL, &ymin, &ymax) != -1 )
#else
		if ( rrd_graph(argc-1, &argv[1], &calcpr, &xsize, &ysize) != -1 )
#endif
		{
			array_init(return_value);
			add_assoc_long(return_value, "xsize", xsize);
			add_assoc_long(return_value, "ysize", ysize);
			add_assoc_long(return_value, "ymin", ymin);
			add_assoc_long(return_value, "ymax", ymax);

			MAKE_STD_ZVAL(p_calcpr);
			array_init(p_calcpr);
    
			if (calcpr) {
				for (i = 0; calcpr[i]; i++) {
					add_next_index_string(p_calcpr, calcpr[i], 1);
					free(calcpr[i]);
				}
				free(calcpr);
			}
			zend_hash_update(return_value->value.ht, "calcpr", sizeof("calcpr"), 
							(void *)&p_calcpr, sizeof(zval *), NULL);
		} else {
			RETVAL_FALSE;
		}

		for (i = 1; i < argc; i++)
			efree(argv[i]);

		efree(argv);
	} else { 
		WRONG_PARAM_COUNT;
	}
	return;
}
/* }}} */

/* {{{ proto mixed rrd_fetch(string file, array args_arr, int p_argc)
 * Fetch info from an RRD file
 */
PHP_FUNCTION(rrd_fetch) {
	pval			 * file, * args, * p_argc;
	pval			 * entry;
	pval			 * p_start, * p_end, * p_step, * p_ds_cnt;
	HashTable		 * args_arr;
	zval			 * p_ds_namv, * p_data, * p_sum;
	int				   i, x, argc;
	time_t			   start, end;
	unsigned long	   step, ds_cnt;
	char			** argv, ** ds_namv; 
	rrd_value_t		 * data, * datap;
    
	if ( rrd_test_error() )
		rrd_clear_error();
    
	if ( ZEND_NUM_ARGS() == 3 && zend_get_parameters(ht, 3, &file, &args, &p_argc) == SUCCESS) {
		if ( args->type != IS_ARRAY ) { 
			php_error(E_WARNING, "2nd Variable passed to rrd_fetch is not an array!\n");
			RETURN_FALSE;
		}
        
		convert_to_long(p_argc);
		convert_to_string(file);

		convert_to_array(args);
		args_arr = args->value.ht;

		argc = p_argc->value.lval + 3;
		argv = (char **) emalloc(argc * sizeof(char *));
 
		argv[0] = "dummy";
		argv[1] = estrdup("fetch");
		argv[2] = estrdup(file->value.str.val);

		for (i = 3; i < argc; i++) {
			pval **dataptr;

			if ( zend_hash_get_current_data(args_arr, (void *) &dataptr) == FAILURE )
				continue;

			entry = *dataptr;

			if ( entry->type != IS_STRING )
				convert_to_string(entry);

			argv[i] = estrdup(entry->value.str.val);

			if ( i < argc )
				zend_hash_move_forward(args_arr);
		}
  
		optind = 0;
		opterr = 0; 

		if ( rrd_fetch(argc-1, &argv[1], &start,&end,&step,&ds_cnt,&ds_namv,&data) != -1 ) {
			array_init(return_value);
			add_assoc_long(return_value, "start", start);
			add_assoc_long(return_value, "end", end);
			add_assoc_long(return_value, "step", step);
			add_assoc_long(return_value, "ds_cnt", ds_cnt);

			MAKE_STD_ZVAL(p_ds_namv);
			MAKE_STD_ZVAL(p_data);
			MAKE_STD_ZVAL(p_sum);
			array_init(p_ds_namv);
			array_init(p_data);
			array_init(p_sum);
   
			if (ds_namv) {
				for (i = 0; i < ds_cnt; i++) {
					add_next_index_string(p_ds_namv, ds_namv[i], 1);
					free(ds_namv[i]);
				}
				free(ds_namv);
			}

			if (data) {
				datap = data;
 
				for (i = start; i <= end; i += step) {
					char	t_time[32] = { 0, };
					char	t_value[1024] = { 0, };
					char	t_sum[2048] = { 0, };
					int		tlen = 0;

					sprintf (t_value, "%10lu: ", i);
					tlen = strlen (t_value);

					for (x = 0; x < ds_cnt; x++) {
						double	tmp;
						char	tmp_value[128] = { 0, };

						tmp = *(datap++);
						add_next_index_double(p_data, tmp);

						sprintf (tmp_value, " %0.10e", tmp);
						memcpy (t_value + tlen, tmp_value, strlen (tmp_value));
						tlen = strlen (t_value);
					}
					add_next_index_string(p_sum, t_value, 1);
				}
 
				free(data);
			}

			zend_hash_update(return_value->value.ht, "ds_namv", sizeof("ds_namv"), 
							(void *)&p_ds_namv, sizeof(zval *), NULL);
			zend_hash_update(return_value->value.ht, "data", sizeof("data"), 
							(void *)&p_data, sizeof(zval *), NULL);
			zend_hash_update(return_value->value.ht, "sum", sizeof("sum"), 
							(void *)&p_sum, sizeof(zval *), NULL);
		} else {
			RETVAL_FALSE;
		}

		for (i = 1; i < argc; i++)
			efree(argv[i]);

		efree(argv);
	} else { 
		WRONG_PARAM_COUNT;
	}
	return;
}
/* }}} */

/* {{{ proto int rrd_dump (string file) 
 * Dump an RRD file with XML format
 */
PHP_FUNCTION(rrd_dump) {
	zval	**  file;
	char	** argv;
	char	 * f;
	int		   argc, i;

	if ( rrd_test_error() )
		rrd_clear_error();

	argc = ZEND_NUM_ARGS();

	switch ( argc ) {
		case 1 :
			if ( zend_get_parameters_ex (argc, &file) == FAILURE )
				WRONG_PARAM_COUNT;

			convert_to_string_ex (file);
			f = Z_STRVAL_PP (file);
			break;
		default :
			WRONG_PARAM_COUNT;
	}

	argv = (char **) emalloc (3 * sizeof (char *));

	argv[0] = estrdup ("dummy");
	argv[1] = estrdup ("dump");
	argv[2] = estrdup (f);

	optind = 0;
	opterr = 0;

	if ( rrd_dump_ex (2, &argv[1]) != -1 ) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}

	for ( i=0; i<3; i++ )
		efree (argv[i]);
	efree(argv);
}
/* }}} */

/* {{{ proto int rrd_restore (string src, string dst, int opts) 
 * Restore an RRD file from XML format
 */
PHP_FUNCTION(rrd_restore) {
	zval	** src_t, ** dst_t, ** opt_t;
	char	** argv, * src, * dst;
	int		   i, argc, opts, optt;

	if ( rrd_test_error() )
		rrd_clear_error();

	argc = ZEND_NUM_ARGS();

	switch ( argc ) {
		case 3 :
			if ( zend_get_parameters_ex (argc, &src_t, &dst_t, &opt_t) == FAILURE )
				WRONG_PARAM_COUNT;

			convert_to_long_ex (opt_t);
			optt = Z_LVAL_PP (opt_t);
			if ( ! optt ) argc--;
			break;
		case 2 :
			if ( zend_get_parameters_ex (argc, &src_t, &dst_t) == FAILURE )
				WRONG_PARAM_COUNT;

			argc--;
			optt = 0;
			break;
		default :
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex (src_t);
	convert_to_string_ex (dst_t);
	src = Z_STRVAL_PP (src_t);
	dst = Z_STRVAL_PP (dst_t);

	opts = argc + 2;

	argv = (char **) emalloc (opts * sizeof (char *));

	i = 0;
	argv[0] = "dummy";
	argv[++i] = estrdup ("restore");
	argc = 1;

	if ( optt > 0 ) {
		argv[++i] = estrdup ("-r");
		argc++;
	}

	argv[++i] = estrdup (src);
	argv[++i] = estrdup (dst);
	argc += 2;

	optind = 0;
	opterr = 0;

	if ( rrd_restore_ex (argc, &argv[1]) != -1 ) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}

	for ( i=1; i<opts; i++ ) {
		efree (argv[i]);
	}
	efree (argv);
}
/* }}} */

/* {{{ Every user-visible function in PHP should document itself in the source
 * proto string confirm_rrdtool_compiled(string arg)
 * Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_rrdtool_compiled)
{
	char	* arg = NULL;
	int		arg_len, len;
	char	string[256];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = sprintf(string, "Congratulations! You have successfully "
						  "modified ext/%.78s/config.m4. Module %.78s "
						  "is now compiled into PHP.", "rrdtool", arg);
	RETURN_STRINGL(string, len, 1);
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
