<?php

function goerr () {
	fprintf (STDERR, "Module %s is not compiled into PHP\n", $GLOBALS['module']);
	exit (1);
}

$module = 'rrd';

if ( ! extension_loaded ('rrd') ) {
	if ( version_compare (PHP_VERSION, '5.4.0', '>=') ) {
		goerr ();
	} else if ( version_compare (PHP_VERSION, '5.3.0', '>=' ) ) {
		if ( ! ini_get ('enable_dl') ) {
			goerr ();
		} else {
			if ( ! dl ('rrd'. PHP_SHLIB_SUFFIX) ) {
				goerr ();
			}
		}
	} else {
		if ( ! ini_get ('enable_dl') ) {
			goerr ();
		} else {
			if ( ! dl ('modules/rrd'. PHP_SHLIB_SUFFIX) ) {
				goerr ();
			}
		}
	}
}

if ( php_sapi_name () == 'cli' )
	$newline = "\n";
else
	$newline = "<br>\n";

$functions = get_extension_funcs ($module);
echo "Functions available in the test extension:\n";
foreach($functions as $func) {
	echo $func.$newline;
}
echo $newline;
$function = 'confirm_' . $module . '_compiled';
echo $function($module) . "\n";
?>
