# qtstalker OI (Open Interest) indicator

$|++;

# create the OI line
$command = "BARS,OI,oi";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set plot style
$command = "INDICATOR_PLOT_STYLE,oi,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set the color
$command = "INDICATOR_PLOT_COLOR,ALL,oi,yellow";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the oi
$command = "INDICATOR_PLOT,oi,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
