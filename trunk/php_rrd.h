/*
 * php_rrd.h
 *
 * php rrdtool extension.  
 *
 * Joe Miller, <joeym@joeym.net>, 7/19/2000 php4-rrdtool 1.0.4
 * JoungKyun Kim, <http://www.oops.org> 7/27/2004 forking to mod_rrd
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
 * $Id$
 *
 */

#ifndef _MOD_RRD_H
#define _MOD_RRD_H

#ifdef PHP_WIN32
#define PHP_RRD_API __declspec(dllexport)
#else
#define PHP_RRD_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_module_entry rrd_module_entry;
#define rrd_module_ptr &rrd_module_entry

// rrd restore option
#define RRD_RANGE_CHECK 0x01
#define RRD_OVER_WRITE  0x02

PHP_MINIT_FUNCTION(rrd);
PHP_MSHUTDOWN_FUNCTION(rrd);
PHP_RINIT_FUNCTION(rrd);
PHP_RSHUTDOWN_FUNCTION(rrd);
PHP_MINFO_FUNCTION(rrd);

PHP_FUNCTION(confirm_rrd_compiled);
PHP_FUNCTION(rrd_error);
PHP_FUNCTION(rrd_clear_error);
PHP_FUNCTION(rrd_update);
PHP_FUNCTION(rrd_last);
PHP_FUNCTION(rrd_create);
PHP_FUNCTION(rrd_graph);
PHP_FUNCTION(rrd_fetch);
PHP_FUNCTION(rrd_dump);
PHP_FUNCTION(rrd_restore);
#ifdef SUPPORT_RRD12
PHP_FUNCTION(rrd_first);
PHP_FUNCTION(rrd_info);
#endif

#ifdef ZTS
#define RRD_G(v) TSRMG(rrd_globals_id, zend_rrd_globals *, v)
#else
#define RRD_G(v) (rrd_globals.v)

#endif

#define RRD_BUILDNO "200709271851"
#define RRD_BUILDVER "1.2.1"

#define phpext_rrd_ptr rrd_module_ptr

#define rrd_parameters(...) \
	    zend_parse_parameters (ZEND_NUM_ARGS() TSRMLS_CC, __VA_ARGS__)

#ifndef safe_efree
#define safe_efree(x) if ( x != NULL ) efree (x)
#endif

#if ZEND_MODULE_API_NO >= 20050922
#	define OB_START_BUFFER php_output_start_default(TSRMLS_C)
#	define OB_GET_BUFFER php_output_get_contents
#	define OB_END_BUFFER php_output_discard(TSRMLS_C)
#else
#	define OB_START_BUFFER php_start_ob_buffer(NULL, 0, 1 TSRMLS_CC)
#	define OB_GET_BUFFER php_ob_get_buffer
#	define OB_END_BUFFER php_ob_end_buffer(0, 0 TSRMLS_CC)
#endif

#endif  /* _MOD_RRD_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
