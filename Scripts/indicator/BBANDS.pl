# qtstalker BBANDS indicator

$|++;

# get close data
$command = "BARS,Close,cl";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get BBANDS
$command = "BBANDS,cl,BB+,BB,BB-,14,SMA,1,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

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

# plot BARS
$command = "INDICATOR_PLOT,Bars,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set BB+ plot style
$command = "INDICATOR_PLOT_STYLE,BB+,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set BB+ color
$command = "INDICATOR_PLOT_COLOR,ALL,BB+,red";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the BB+
$command = "INDICATOR_PLOT,BB+,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set BB plot style
$command = "INDICATOR_PLOT_STYLE,BB,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot BB color
$command = "INDICATOR_PLOT_COLOR,ALL,BB,yellow";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot BB
$command = "INDICATOR_PLOT,BB,2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set BB- plot style
$command = "INDICATOR_PLOT_STYLE,BB-,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set BB- color
$command = "INDICATOR_PLOT_COLOR,ALL,BB-,green";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the BB-
$command = "INDICATOR_PLOT,BB-,3";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
