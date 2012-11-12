--TEST--
Check for rrd_restore functoin
--SKIPIF--
<?php
if (!extension_loaded("rrd")) {
  if ( ! @dl ("modules/rrd.so") )
    print "skip";
}
?>
--POST--
--GET--
--INI--
--FILE--
<?
if (!extension_loaded("rrd")) {
  dl ("./modules/rrd.so");
}

$xmlformat = "./tests/test.xml";
$rrddb     = "/tmp/rrd-test.rrd";
$option    = 0;

system ("rrdtool dump ./tests/test.rrd > $xmlformat");
rrd_restore ($xmlformat, $rrddb, $option);
//rrd_restore ($xmlformat, $rrddb);

$err = rrd_error();
if ( $err )
  echo $err ."\n";
else
  echo "rrd_restore function is available";

if ( file_exists ($xmlformat) )
  unlink ($xmlformat);

if ( file_exists ($rrddb) )
  unlink ($rrddb);
?>
--EXPECT--
rrd_restore function is available
