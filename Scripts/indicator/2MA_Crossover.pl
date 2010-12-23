# qtstalker 2MA_Crossover (OHLC Bars, 50 period SMA, 200 period SMA) indicator

$|++;

# create the BARS indicator
$command = "BARS,BARS,Bars";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

# color bars red if Bars close < previous Bars close
$command = "INDICATOR_PLOT_COLOR,COMPARE_INDEX_ALL,Bars.0,<,Bars.1,Bars.0,red";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# color bars blue if Bars close == previous Bars close
$command = "INDICATOR_PLOT_COLOR,COMPARE_INDEX_ALL,Bars.0,=,Bars.1,Bars.0,blue";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# color bars green if Bars close > previous Bars close
$command = "INDICATOR_PLOT_COLOR,COMPARE_INDEX_ALL,Bars.0,>,Bars.1,Bars.0,green";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the 50 EMA indicator
$command = "MA,EMA,50MA,Bars,50";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

# create the 200 EMA indicator
$command = "MA,EMA,200MA,Bars,200";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

# plot the BARS line
$command = "INDICATOR_PLOT,Bars,0";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

# set 50MA plot style
$command = "INDICATOR_PLOT_STYLE,50MA,Line";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

# set 50MA color
$command = "INDICATOR_PLOT_COLOR,ALL,50MA,yellow";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

# plot the 50MA
$command = "INDICATOR_PLOT,50MA,1";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

# set 200MA plot style
$command = "INDICATOR_PLOT_STYLE,200MA,Line";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

# set 200MA color
$command = "INDICATOR_PLOT_COLOR,ALL,200MA,blue";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

# plot the 200MA
$command = "INDICATOR_PLOT,200MA,2";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }
