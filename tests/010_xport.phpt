--TEST--
Check for rrd_xport function
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
$opt = array (
    '--start', 'now-1h', '--end', 'now',
    "DEF:xx={$rrddb}:RX:AVERAGE",
    "DEF:yy={$rrddb}:TX:AVERAGE",
    'CDEF:aa=xx,yy,+,8,*',
    'XPORT:xx:"out bytes"',
    'XPORT:aa:"in and out bits'
);

ob_start ();
rrd_xport ($opt, count ($opt));
$ret = ob_get_contents ();
ob_end_clean ();

$err = rrd_error ();
if ( $err )
	echo $err;
else
	echo 'rrd_xport function is available';

//if ( file_exists ($rrddb) )
//	unlink ($rrddb);

?>
--EXPECT--
rrd_xport function is available
