dnl $Id: config.m4,v 1.1.1.1 2004-07-28 05:56:08 oops Exp $

PHP_ARG_WITH(rrd, for RRDTool support,
[  --with-rrd[=DIR]      Include RRDTool support.  DIR is the rrdtool
                          install directory.])

if test "$PHP_RRD" != "no"; then
  AC_DEFINE(HAVE_RRD,1,[ ])

  RRD_PARAMETER=$CFLAGS
  PHP_SUBST(CPPFLAGS)
  PHP_SUBST(LDFLAGS)

  SEARCH_PATH="/usr /usr/local /opt/rrdtool /usr/local/rrdtool $PHP_RRD"

  for i in $SEARCH_PATH; do
    if test -f $i/include/rrd.h; then
      RRD_HEADER_DIR=$i/include
      break
    fi
  done

  if test -z "$RRD_HEADER_DIR"; then
    HEASEARCH=`echo $CPPFLAGS | sed 's/-I//g'`
    for i in $HEASEARCH; do
      if test -f $i/rrd.h; then
        RRD_HEADER_DIR=$i
        break
      fi
    done

    if test -z "$RRD_HEADER_DIR"; then
      AC_MSG_ERROR([Unable to find rrd.h anywhere under $CPPFLAGS $SEARCH_PATH])
    fi
  fi

  for i in $SEARCH_PATH; do
    if test -f $i/lib/rrd.$SHLIB_SUFFIX_NAME -o -f $i/lib/librrd.a ; then
      RRD_LIB_DIR=$i/lib
      break
    fi
  done
  
  if test -z "$RRD_LIB_DIR"; then
    LIBSEARCH=`echo $LDFLAGS | sed 's/-L//g'`

    for i in $LIBSEARCH; do
      if test -f $i/librrd.$SHLIB_SUFFIX_NAME -o -f $i/librrd.a; then
        RRD_LIB_DIR=$i
        break
      fi
    done
    
    if test -z "$RRD_LIB_DIR"; then
      AC_MSG_ERROR([librrd.(a|so) not found.])
    fi
  fi

  dnl Checks for header files.
  AC_HEADER_STDC
  AC_CHECK_HEADERS(sys/stat.h sys/types.h fcntl.h locale.h fp_class.h malloc.h unistd.h ieeefp.h math.h time.h sys/time.h sys/times.h sys/param.h sys/resource.h float.h)

  dnl Checks for library functions.
  AC_FUNC_STRFTIME
  AC_FUNC_VPRINTF

  extra_src="rrdlib/rrd_dump.c rrdlib/rrd_restore.c"

  PHP_EXPAND_PATH($RRD_HEADER_DIR, RRD_HEADER_DIR)
  PHP_ADD_INCLUDE($RRD_HEADER_DIR)
  PHP_ADD_LIBRARY_WITH_PATH(rrd, $RRD_LIB_DIR, RRD_SHARED_LIBADD)
  PHP_SUBST(RRD_PARAMTER)
  PHP_SUBST(RRD_SHARED_LIBADD)


  PHP_NEW_EXTENSION(rrd, rrd.c $extra_src, $ext_shared,, \\$(RRD_PARAMETER))
fi
