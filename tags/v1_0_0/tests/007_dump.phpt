--TEST--
Check for rrd_dump function
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
  dl ("modules/rrd.so");
}

$filename = "tests/test.rrd";

ob_start();
rrd_dump ($filename);
$ret = ob_get_contents();
ob_end_clean();

$err = rrd_error();
if ( $err )
  echo $err;
else
  echo "rrd_dump function is available";

?>
--EXPECT--
rrd_dump function is available
