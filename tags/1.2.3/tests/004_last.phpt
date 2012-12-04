--TEST--
Check for rrd_last function 
--SKIPIF--
<?php
if ( ! extension_loaded ('rrd') ) {
	print 'skip';
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

$rrddb = './tests/sample.rrd';
$ret = rrd_last ($rrddb);

if ( $ret != -1 ) {
	echo 'rrd_last function is available';
	//printf ("Last update time:  %s\n", strftime("%m/%d/%Y %H:%M:%S"), $ret);
} else {
	$err_msg = rrd_error ();
	echo "Error occurred:  $err_msg\n";
}
?>
--EXPECT--
rrd_last function is available
