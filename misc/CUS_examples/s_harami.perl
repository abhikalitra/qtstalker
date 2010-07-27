# qtstalker s_harami - Candles: Harami and Harami Cross

# yellow  : Harami
# orange  : Harami Cross
#
# Positive values are bullish, negative values are bearish.

$|++;

# get Harami
print STDOUT "INDICATOR,PLUGIN,CANDLES,HARAMI,c1,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Harami Cross
print STDOUT "INDICATOR,PLUGIN,CANDLES,HARAMICROSS,c2,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set harami plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,c1,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

#set the harami color
print STDOUT "INDICATOR,SET_COLOR_ALL,c1,yellow";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# plot the harami 
print STDOUT "INDICATOR,SET_PLOT,c1";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }


# set harami cross plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,c2,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

#set the harami cross color
print STDOUT "INDICATOR,SET_COLOR_ALL,c2,orange";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# plot the harami cross
print STDOUT "INDICATOR,SET_PLOT,c2";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

