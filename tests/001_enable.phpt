--TEST--
Check for rrdtool presence
--SKIPIF--
<?php
if ( ! extension_loaded ('rrd') ) {
	if ( version_compare (PHP_VERSION, '5.4.0', '<') ) {
		print 'skip';
	} else if ( version_compare (PHP_VERSION, '5.3.0', '<') ) {
		if ( ! @dl ('rrd.so') )
			print 'skip';
	} else {
		if ( ! @dl ('./modules/rrd.so') )
			print 'skip';
	}
}
?>
--POST--
--GET--
--INI--
--FILE--
<?php
echo "RRDTool extension is available";
/*
        you can add regression tests for your extension here

  the output of your test code has to be equal to the
  text in the --EXPECT-- section below for the tests
  to pass, differences between the output and the
  expected text are interpreted as failure

        see php4/README.TESTING for further information on
  writing regression tests
*/
?>
--EXPECT--
RRDTool extension is available
