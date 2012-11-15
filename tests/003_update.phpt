--TEST--
Check for rrd_update function
--SKIPIF--
<?php
if ( ! extension_loaded ('rrd') ) {
	print 'skip';
}
--POST--
--GET--
--INI--
--FILE--
<?php
##
## demonstration of the rrd_update() command
##

$rrddb = './tests/sample.rrd';
$ret = rrd_update ($rrddb, "N:1297:98344");

if ( $ret == 0 ) {
	$err = rrd_error ();
	echo "ERROR occurred: $err\n";
} else
	echo 'rrd_update function is available';

/* else rrd_update() was successful */
?>
--EXPECT--
rrd_update function is available
