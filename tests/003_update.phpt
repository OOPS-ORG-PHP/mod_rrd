--TEST--
Check for rrd_update function
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
