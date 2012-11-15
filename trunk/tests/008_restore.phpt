--TEST--
Check for rrd_restore functoin
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
$rrdtdb = '/tmp/rrd-test.rrd';
$option = 0;

if ( ! file_exists ($rrdxml) )
	system ("rrdtool dump {$rrddb} > {$rrdxml}");
rrd_restore ($rrdxml, $rrdtdb, $option);

$err = rrd_error ();
if ( $err )
	echo $err ."\n";
else
	echo 'rrd_restore function is available';

if ( file_exists ($rrddb) )
	unlink ($rrddb);

if ( file_exists ($rrdxml) )
	unlink ($rrdxml);

if ( file_exists ($rrdtdb) )
	unlink ($rrdtdb);
?>
--EXPECT--
rrd_restore function is available
