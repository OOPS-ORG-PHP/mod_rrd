--TEST--
Check for rrd_last function 
--SKIPIF--
<?php
if ( ! extension_loaded ('rrd') ) {
	if ( version_compare (PHP_VERSION, '5.4.0', '>=') ) {
		print 'skip';
	} else if ( version_compare (PHP_VERSION, '5.3.0', '>=') ) {
		if ( ! @dl ('rrd.so') )
			print 'skip';
	} else {
		if ( ! @dl ('./modules/rrd.so') )
			print 'skip';
	}
}
?>
--POST--
--GET--
--INI--
--FILE--
<?php
##
## demonstration of the rrd_last() command
##

$ret = rrd_last ("./tests/test.rrd");

if ( $ret != -1 ) {
	echo "rrd_last function is available";
	//printf ("Last update time:  %s\n", strftime("%m/%d/%Y %H:%M:%S"), $ret);
} else {
	$err_msg = rrd_error ();
	echo "Error occurred:  $err_msg\n";
}
?>
--EXPECT--
rrd_last function is available
