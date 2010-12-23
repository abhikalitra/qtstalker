# qtstalker BARS (OHLC Bars) indicator

$|++;

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

# plot Bars
$command = "INDICATOR_PLOT,Bars,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
