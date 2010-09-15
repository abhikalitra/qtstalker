# qtstalker s_harami - Candles: Harami and Harami Cross

# yellow  : Harami
# orange  : Harami Cross
#
# Positive values are bullish, negative values are bearish.

$|++;

# get Harami
print STDOUT "INDICATOR,PLUGIN,CANDLES,HARAMI,Harami,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Harami Cross
print STDOUT "INDICATOR,PLUGIN,CANDLES,HARAMICROSS,Harami Cross,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set harami plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,Harami,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

#set the harami color
print STDOUT "INDICATOR,SET_COLOR_ALL,Harami,yellow";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# plot the harami 
print STDOUT "INDICATOR,SET_PLOT,Harami,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }


# set harami cross plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,Harami Cross,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

#set the harami cross color
print STDOUT "INDICATOR,SET_COLOR_ALL,Harami Cross,orange";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# plot the harami cross
print STDOUT "INDICATOR,SET_PLOT,Harami Cross,1";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

