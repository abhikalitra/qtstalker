# qtstalker day 20SMA 50SMA 200SMA ( day, 20 period SMA, 50 period SMA, 200 period SMA) indicator
# created 13-09-2009 x_curve
# colors are in #xxxxxx format or in plain english
#plot the chart in this order 200, 50, 20, day so day overlays the others

$|++;


# create the day indicator
print STDOUT "INDICATOR,NEW,Close,Day,Line,red";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 20 SMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,SMA,20MA,Close,20,Dash,#09ff00";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 50 SMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,SMA,50MA,Close,50,Dash,#9041ff";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 200 SMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,SMA,200MA,Close,200,Dot,#f3ff88";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot 200SMA - 50SMA - 20SMA - Day, day is on the foreground

# plot 200 MA
print STDOUT "PLOT,200MA";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

#plot 50 SMA
print STDOUT "PLOT,50MA";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

#plot 20 SMA
print STDOUT "PLOT,20MA";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot day
print STDOUT "PLOT,Day";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

