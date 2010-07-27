# qtstalker 3MA_Crossover (OHLC Bars, 10 period SMA, 20 period SMA, 50 period SMA) indicator

$|++;

# create the BARS indicator
print STDOUT "INDICATOR,PLUGIN,BARS,Bars,green,red,blue";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# get some close bars
print STDOUT "INDICATOR,NEW,Close,cl";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 10 EMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,EMA,10MA,cl,10";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 20 EMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,EMA,20MA,cl,20";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 50 EMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,EMA,50MA,cl,50";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot BARS
print STDOUT "INDICATOR,SET_PLOT,Bars";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set 10MA plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,10MA,Line";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set 10MA color
print STDOUT "INDICATOR,SET_COLOR_ALL,10MA,blue";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot the 10MA
print STDOUT "INDICATOR,SET_PLOT,10MA";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set 20MA plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,20MA,Line";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot 20MA color
print STDOUT "INDICATOR,SET_COLOR_ALL,20MA,red";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot 20MA
print STDOUT "INDICATOR,SET_PLOT,20MA";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set 50MA plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,50MA,Line";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set 50MA color
print STDOUT "INDICATOR,SET_COLOR_ALL,50MA,yellow";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot the 50MA
print STDOUT "INDICATOR,SET_PLOT,50MA";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

