# qtstalker AROON indicator

$|++;

# create the AROON
$command = "AROON,AROON+,AROON-,14";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set upper plot style
$command = "INDICATOR_PLOT_STYLE,AROON+,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set upper color
$command = "INDICATOR_PLOT_COLOR,ALL,AROON+,green";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the upper
$command = "INDICATOR_PLOT,AROON+,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set lower plot style
$command = "INDICATOR_PLOT_STYLE,AROON-,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set lower color
$command = "INDICATOR_PLOT_COLOR,ALL,AROON-,red";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the lower
$command = "INDICATOR_PLOT,AROON-,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
