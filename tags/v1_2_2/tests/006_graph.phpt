--TEST--
Check for rrd_graph function
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
 ## demonstration of the rrd_graph() command
 ##

   $opts = array( "--start", "-4d", 
                  "DEF:in=./tests/test.rrd:RX:AVERAGE",
                  "DEF:out=./tests/test.rrd:TX:AVERAGE",
                  "LINE2:in#0000ff:Traffic Avg.",
                  "PRINT:in:AVERAGE:RX\: %1.2lf b/s",
                  "PRINT:in:AVERAGE:TX\: %1.2lf b/s"
                );


   $tmpfile = "/tmp/rrd-test.png";
   $ret = rrd_graph($tmpfile, $opts, count($opts));

   ##
   ## if $ret is an array, then rrd_graph was successful
   ##
   if ( is_array($ret) )
   {
       echo "rrd_graph function is available";
       /*
       echo "Image size:  $ret[xsize] x $ret[ysize]\n";
       

       ##
       ## all results from any PRINT commands will be
       ## in the array $ret[calcpr][..]
       ##
       echo "rrd_graph1 print results: \n";

       for ($i = 0; $i < count($ret[calcpr]); $i++)
           echo $ret[calcpr][$i] . "\n";
       */
   }
   else
   {
       $err = rrd_error();
       echo "rrd_graph() ERROR: $err\n";
   }

   if ( file_exists ($tmpfile) )
     unlink ($tmpfile);

?>
--EXPECT--
rrd_graph function is available
