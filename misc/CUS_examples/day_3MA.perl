# qtstalker day 20SMA 50SMA 200SMA ( day, 20 period SMA, 50 period SMA, 200 period SMA) indicator
# created 13-09-2009 x_curve
# colors are in #xxxxxx format or in plain english
#plot the chart in this order 200, 50, 20, day so day overlays the others

$|++;


# create the day indicator
print STDOUT "INDICATOR,NEW,Close,Day";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 20 SMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,SMA,20MA,Close,20";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 50 SMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,SMA,50MA,Close,50";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# create the 200 SMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,SMA,200MA,Close,200";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot 200SMA - 50SMA - 20SMA - Day, day is on the foreground

# set 200MA plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,200MA,Dot";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

print STDOUT "INDICATOR,SET_COLOR_ALL,200MA,#f3ff88";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot 200 MA
print STDOUT "INDICATOR,SET_PLOT,200MA,0";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set 50MA plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,50MA,Line";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set 50MA color
print STDOUT "INDICATOR,SET_COLOR_ALL,50MA,#9041ff";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

#plot 50MA
print STDOUT "INDICATOR,SET_PLOT,50MA,1";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

#set 20MA plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,20MA,Line";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set 20MA color
print STDOUT "INDICATOR,SET_COLOR_ALL,20MA,#09ff00";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

#plot 20 SMA
print STDOUT "INDICATOR,SET_PLOT,20MA,2";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set Day plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,Day,Line";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set Day color
print STDOUT "INDICATOR,SET_COLOR_ALL,Day,red";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot Day
print STDOUT "INDICATOR,SET_PLOT,Day,3";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

