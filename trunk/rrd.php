<?
if ( ! extension_loaded ('rrd') ) {
  dl ('modules/rrd.' . PHP_SHLIB_SUFFIX);
}

$module = 'rrd';

$CLIOPT = getenv ('TEST_PHP_EXECUTABLE');

$newline = ($CLIOPT) ? "\n" : "<br>\n";

$functions = get_extension_funcs ($module);
echo "Functions available in the test extension:\n";
foreach($functions as $func) {
    echo $func.$newline;
}
echo $newline;
$function = 'confirm_' . $module . '_compiled';
if (extension_loaded($module)) {
        $str = $function($module);
} else {
        $str = "Module $module is not compiled into PHP";
}
echo "$str\n";
?>
