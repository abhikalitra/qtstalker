# qtstalker RSI indicator

$|++;

# create the Close line
$command = "BARS,Close,c";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the RSI of the close
$command = "RSI,RSI,c,14";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the buy horizontal line
$command = "CHART_OBJECT_HLINE,RO,30,green";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the sell horizontal line
$command = "CHART_OBJECT_HLINE,RO,70,red";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set plot style
$command = "INDICATOR_PLOT_STYLE,RSI,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set the color
$command = "INDICATOR_PLOT_COLOR,ALL,RSI,red";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the oi
$command = "INDICATOR_PLOT,RSI,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
