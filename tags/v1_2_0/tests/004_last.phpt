--TEST--
Check for rrd_last function 
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
 ## demonstration of the rrd_last() command
 ##

 $ret = rrd_last("./tests/test.rrd");

 if ( $ret != -1 )
 {
     echo "rrd_last function is available";
     //printf("Last update time:  %s\n", strftime("%m/%d/%Y %H:%M:%S"), $ret);
 }
 else
 {
     $err_msg = rrd_error();
     echo "Error occurred:  $err_msg\n";
 }


?>
--EXPECT--
rrd_last function is available
