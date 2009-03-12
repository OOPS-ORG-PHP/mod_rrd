/*****************************************************************************
 * RRDtool 1.0.48  Copyright Tobias Oetiker, 1997 - 2000
 *****************************************************************************
 * rrd_restore.c  creates new rrd from data dumped by rrd_dump.c
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <stdarg.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#if HAVE_MATH_H
# include <math.h>
#endif
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#if HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#if HAVE_SYS_TIMES_H
# include <sys/times.h>
#endif
#if HAVE_SYS_RESOURCE_H
# include <sys/resource.h>
#if (defined(__svr4__) && defined(__sun__))
/* Solaris headers (pre 2.6) don't have a getrusage prototype.
 *    Use this instead. */
extern int getrusage(int, struct rusage *);
#endif /* __svr4__ && __sun__ */
#endif

/* unix-only includes */
#ifndef isnan
int isnan(double value);
#endif

#include "php.h"
#include "php_ini.h"
#include "zend_API.h"

#include "rrd.h"
#include "rrd_format.h"

/* Prototypes */

int xml2rrd(char*, rrd_t*, char);
int rrd_write(char *, rrd_t *);
int readfile_ex (unsigned char *filename, char **buf);

/* create and empty rrd file according to the specs given */

int rrd_restore_ex (int argc, char **argv) 
{
	rrd_t	  rrd;
	char	* buf;
	char	  rc = 0;

	/* init rrd clean */
	rrd_init (&rrd);
	if ( argc < 3 ) {
		rrd_set_error ("usage rrdtool %s [--range-check/-r] file.xml file.rrd", argv[0]);
		return -1;
	}
	
	while (1) {
		static struct option long_options[] =
		{
			{"range-check",      required_argument, 0,  'r'},
			{0,0,0,0}
		};
		int option_index = 0;
		int opt;

		opt = getopt_long (argc, argv, "r", long_options, &option_index);
		
		if (opt == EOF) break;
		
		switch(opt) {
			case 'r':
				rc = 1;
				break;
			default:
				rrd_set_error ("usage rrdtool %s [--range-check|-r] file.xml file.rrd", argv[0]);
				return -1;
				break;
		}
	}
	
	if ( readfile_ex(argv[optind], &buf) == -1 ) {
		return -1;
	}

	if ( xml2rrd(buf,&rrd,rc) == -1 ) {
		rrd_free (&rrd);
		free (buf);
		return -1;
	}
	free (buf);

	if ( rrd_write (argv[optind+1], &rrd) == -1 ) {
		rrd_free (&rrd);	
		return -1;	
	};
	rrd_free (&rrd);    

	return 0;
}

int readfile_ex (unsigned char *filename, char **buf ) {
	struct stat filebuf;

	FILE	* fp;
	size_t	filesize = 0, frsize = 0, len = 0, strlength = 0;
	static unsigned char	tmptext[2049];

	// get file info
	stat (filename, &filebuf);
	// original file size
	filesize = filebuf.st_size;

	if ( (fp = fopen (filename, "rb")) == NULL) {
			php_error (E_WARNING, "Can't open %s in read mode", filename);
			return -1;
	}

	*buf = (char *) malloc (sizeof (char) * (filesize + 32));
	memset (tmptext, 0, sizeof (tmptext));

	while ( (len = fread (tmptext, sizeof (char), 2048, fp)) > 0 ) {
		tmptext[len] = 0;
		memmove (*buf + strlength, tmptext, len);
		strlength += len;
	}

	if ( strlength < filesize ) { filesize = strlength; }
	memset (*buf + filesize, 0, 1);

	fclose (fp);

	return 0;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
