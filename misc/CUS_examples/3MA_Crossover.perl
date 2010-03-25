# qtstalker 3MA_Crossover (OHLC Bars, 10 period SMA, 20 period SMA, 50 period SMA) indicator

$|++;

# create the BARS indicator
print STDOUT "INDICATOR,PLUGIN,BARS,Bars,green,red,blue";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# get some close bars
print STDOUT "INDICATOR,PLUGIN,REF,cl,Close,0";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 10 SMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,SMA,10MA,cl,10";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 20 SMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,SMA,20MA,cl,20";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 50 SMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,SMA,50MA,cl,50";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot BARS
print STDOUT "PLOT,Bars,C,OHLC";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# color the 10MA line prior to plotting
print STDOUT "INDICATOR,PLUGIN,COLOR,All,10MA,blue";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot 10 SMA
print STDOUT "PLOT,10MA,10MA,Line";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# color the 20MA line prior to plotting
print STDOUT "INDICATOR,PLUGIN,COLOR,All,20MA,red";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot 20 SMA
print STDOUT "PLOT,20MA,20MA,Line";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# color the 50MA line prior to plotting
print STDOUT "INDICATOR,PLUGIN,COLOR,All,50MA,yellow";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot 50 SMA
print STDOUT "PLOT,50MA,50MA,Line";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }


