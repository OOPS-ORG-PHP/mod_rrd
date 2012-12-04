--TEST--
Check for rrd_restore functoin
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
$rrddb = './tests/sample.rrd';
$rrdxml = '/tmp/rrd-test.xml';
$rrdtdb = '/tmp/rrd-test.rrd';
#$option = 0;
#$option = RRD_RANGE_CHECK;
#$option = RRD_OVER_WRITE;
$option = RRD_RANGE_CHECK|RRD_OVER_WRITE;

if ( ! file_exists ($rrdxml) )
	system ("rrdtool dump {$rrddb} > {$rrdxml}");
rrd_restore ($rrdxml, $rrdtdb, $option);

$err = rrd_error ();
if ( $err )
	echo $err ."\n";
else
	echo 'rrd_restore function is available';

//if ( file_exists ($rrddb) )
//	unlink ($rrddb);

if ( file_exists ($rrdxml) )
	unlink ($rrdxml);

if ( file_exists ($rrdtdb) )
	unlink ($rrdtdb);
?>
--EXPECT--
rrd_restore function is available
