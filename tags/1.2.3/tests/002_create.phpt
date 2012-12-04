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
##
## demonstration of the rrd_create() command
##
$opts = array( "--step", "300", "--start", 0,
		"DS:input:COUNTER:900:0:U",
		"DS:output:COUNTER:900:0:U",
		"RRA:AVERAGE:0.5:1:1000",
		"RRA:MIN:0.5:1:1000",
		"RRA:MAX:0.5:1:1000"
);

$rrddb = '/tmp/test.rrd';
$ret = rrd_create ($rrddb, $opts, count($opts));

if ( $ret == 0 ) {
	$err = rrd_error();
	echo "Create error: $err";
} else {
	echo 'rrd_create function is available';
	unlink ($rrddb);
	if ( ! file_exists ('./tests/sample.rrd') )
		copy ('./tests/test.rrd', './tests/sample.rrd');
}

/*  else rrd_create was successful  */
?>
--EXPECT--
rrd_create function is available
