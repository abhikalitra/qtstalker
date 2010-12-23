# qtstalker 3MA_Crossover (OHLC Bars, 10 period SMA, 20 period SMA, 50 period SMA) indicator

$|++;

# create the BARS indicator
$command = "BARS,BARS,Bars";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

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

# create the 10 EMA indicator
$command = "MA,EMA,10MA,Bars,10";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the 20 EMA indicator
$command = "MA,EMA,20MA,Bars,20";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the 50 EMA indicator
$command = "MA,EMA,50MA,Bars,50";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot BARS
$command = "INDICATOR_PLOT,Bars,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set 10MA plot style
$command = "INDICATOR_PLOT_STYLE,10MA,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set 10MA color
$command = "INDICATOR_PLOT_COLOR,ALL,10MA,blue";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the 10MA
$command = "INDICATOR_PLOT,10MA,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set 20MA plot style
$command = "INDICATOR_PLOT_STYLE,20MA,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot 20MA color
$command = "INDICATOR_PLOT_COLOR,ALL,20MA,red";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot 20MA
$command = "INDICATOR_PLOT,20MA,2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set 50MA plot style
$command = "INDICATOR_PLOT_STYLE,50MA,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set 50MA color
$command = "INDICATOR_PLOT_COLOR,ALL,50MA,yellow";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the 50MA
$command = "INDICATOR_PLOT,50MA,3";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
