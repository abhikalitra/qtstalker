# qtstalker 2MA_Crossover (OHLC Bars, 50 period SMA, 200 period SMA) indicator

$|++;

# create the BARS indicator
print STDOUT "INDICATOR,PLUGIN,BARS,Bars,green,red,blue";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# get some close bars
print STDOUT "INDICATOR,NEW,Close,cl";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 50 EMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,EMA,50MA,cl,50";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 200 EMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,EMA,200MA,cl,200";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot the BARS line
print STDOUT "INDICATOR,SET_PLOT,Bars";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set 50MA plot style
print STDOUT "INDICATOR,SET_PLOT,50MA,Line";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set 50MA color
print STDOUT "INDICATOR,SET_COLOR_ALL,50MA,yellow";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot the 50MA
print STDOUT "INDICATOR,SET_PLOT,50MA";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set 200MA plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,200MA,Line";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set 200MA color
print STDOUT "INDICATOR,SET_COLOR_ALL,200MA,blue";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot the 200MA
print STDOUT "INDICATOR,SET_PLOT,200MA";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

