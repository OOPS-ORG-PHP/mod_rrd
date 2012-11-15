--TEST--
Check for rrd_dump function
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
$rrddb = './tests/sample.rrd';
$rrdxml = '/tmp/rrd-test.xml';

ob_start ();
rrd_dump ($rrddb);
$ret = ob_get_contents ();
ob_end_clean ();

$err = rrd_error ();
if ( $err )
	echo $err;
else {
	echo 'rrd_dump function is available';
	file_put_contents ($rrdxml, $ret);
}

//if ( file_exists ($rrddb) )
//	unlink ($rrddb);

?>
--EXPECT--
rrd_dump function is available
