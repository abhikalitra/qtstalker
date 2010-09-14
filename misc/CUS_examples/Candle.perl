# qtstalker Candle indicator
# NOTE: we dont need to set the plot type because the CANDLES plugin does it for you

$|++;

# get the Candle indicator
print STDOUT "INDICATOR,PLUGIN,CANDLES,NONE,candles";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set the color
print STDOUT "INDICATOR,SET_COLOR_ALL,candles,green";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot it
print STDOUT "INDICATOR,SET_PLOT,candles,0";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

