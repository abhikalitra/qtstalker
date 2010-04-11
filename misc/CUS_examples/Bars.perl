# qtstalker BARS (OHLC Bars) indicator

$|++;

print STDOUT "INDICATOR,PLUGIN,BARS,Bars,green,red,blue"; # create the BARS indicator
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

print STDOUT "PLOT,Bars,C,OHLC"; # plot BARS
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

