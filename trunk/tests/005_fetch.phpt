--TEST--
Check for rrd_create function
--SKIPIF--
<?php
if (! extension_loaded ("rrd")) {
  if ( ! @dl ("modules/rrd.so") )
    print "skip";
}
?>
--POST--
--GET--
--INI--
--FILE--
<?
if (! extension_loaded ("rrd")) {
  dl ("modules/rrd.so");
}

$now = time();
$sta = $now - ( 86400 * 45 );

$file = "tests/test.rrd";
$opt = array ("MAX", "--start", $sta, "--end", $now);

$x = rrd_fetch ($file, $opt, count ($opt));

$err = rrd_error ();
if ( $err )
  echo $err;
else
  echo "rrd_fetch function is available";

#print_r ($x);

?>
--EXPECT--
rrd_fetch function is available
