--TEST--
Check for rrd_info function
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

ob_start ();
rrd_info ($rrddb);
$ret = ob_get_contents ();
ob_end_clean ();

$err = rrd_error ();
if ( $err )
	echo $err;
else
	echo 'rrd_info function is available';

if ( file_exists ($rrddb) )
	unlink ($rrddb);

?>
--EXPECT--
rrd_info function is available
