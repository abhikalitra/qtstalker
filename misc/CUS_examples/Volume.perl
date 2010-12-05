# qtstalker Volume (Volume) indicator, red is down, blue is neutral, green is up, color based on close price.

$|++;

# get the volume data
$command = "BARS,Volume,Volume";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get close data
$command = "BARS,Close,cl";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# color volume bars red if close < previous close
$command = "INDICATOR_PLOT_COLOR,COMPARE_INDEX_ALL,cl.0,<,cl.1,Volume.0,red";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# color volume bars blue if close == previous close
$command = "INDICATOR_PLOT_COLOR,COMPARE_INDEX_ALL,cl.0,=,cl.1,Volume.0,blue";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# color volume bars green if close > previous close
$command = "INDICATOR_PLOT_COLOR,COMPARE_INDEX_ALL,cl.0,>,cl.1,Volume.0,green";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set the plot style to Histogram Bar
$command = "INDICATOR_PLOT_STYLE,Volume,Histogram Bar";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the Volume indicator
$command = "INDICATOR_PLOT,Volume,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the 10 EMA indicator
$command = "MA,EMA,10MA,Volume,10";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set 10MA plot style
$command = "INDICATOR_PLOT_STYLE,10MA,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set 10MA color
$command = "INDICATOR_PLOT_COLOR,ALL,10MA,yellow";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the 10MA
$command = "INDICATOR_PLOT,10MA,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
