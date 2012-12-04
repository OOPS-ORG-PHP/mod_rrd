--TEST--
Check for rrd_create function
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
$now = time ();
$sta = $now - (86400 * 45);

$rrddb = './tests/sample.rrd';
$opt = array ('MAX', '--start', $sta, '--end', $now);

$x = rrd_fetch ($rrddb, $opt, count ($opt));

$err = rrd_error ();
if ( $err )
	echo $err;
else
	echo 'rrd_fetch function is available';

#print_r ($x);

?>
--EXPECT--
rrd_fetch function is available
