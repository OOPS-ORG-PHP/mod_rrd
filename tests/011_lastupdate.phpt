--TEST--
Check for rrd_lastupdate function
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

$r = rrd_lastupdate ($rrddb);

$err = rrd_error ();
if ( $err )
	echo $err;
else
	echo 'rrd_lastupdate function is available';

if ( file_exists ($rrddb) )
	unlink ($rrddb);

?>
--EXPECT--
rrd_lastupdate function is available
