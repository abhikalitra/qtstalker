# qtstalker 3MA_Crossover (OHLC Bars, 10 period SMA, 20 period SMA, 50 period SMA) indicator

$|++;

# create the BARS indicator
print STDOUT "INDICATOR,PLUGIN,BARS,Bars,green,red,blue";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# get some close bars
print STDOUT "INDICATOR,NEW,Close,cl,Line,red";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 10 EMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,EMA,10MA,cl,10,Line,blue";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 20 EMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,EMA,20MA,cl,20,Line,red";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 50 EMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,EMA,50MA,cl,50,Line,yellow";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot BARS
print STDOUT "PLOT,Bars";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot 10 SMA
print STDOUT "PLOT,10MA";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot 20 SMA
print STDOUT "PLOT,20MA";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot 50 SMA
print STDOUT "PLOT,50MA";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }


