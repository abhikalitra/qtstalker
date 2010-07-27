# qtstalker BARS (OHLC Bars) indicator

$|++;

print STDOUT "INDICATOR,PLUGIN,BARS,OHLC,green,red,blue"; # create the BARS indicator
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

print STDOUT "INDICATOR,SET_PLOT,OHLC"; # plot BARS
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

