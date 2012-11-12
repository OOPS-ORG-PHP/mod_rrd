--TEST--
Check for rrd_update function
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
<?php

 ## the following line is only needed if built as a self-contained
 ## extension.  If you build the rrdtool module as an embedded
 ## extension, the rrd_* functions will always be available, so you
 ## do not need the dl() call.
 if (!extension_loaded("rrd")) {
   dl ("modules/rrd.so");
 }

 ##
 ## demonstration of the rrd_update() command
 ##

  $ret = rrd_update("./tests/test.rrd", "N:1297:98344");

  if ( $ret == 0 )
  {
      $err = rrd_error();
      echo "ERROR occurred: $err\n";
  } else {
    echo "rrd_update function is available";
  }
 /* else rrd_update() was successful */


?>
--EXPECT--
rrd_update function is available
