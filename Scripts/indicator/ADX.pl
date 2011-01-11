# qtstalker ADX indicator

$|++;

# create the MDI
$command = "MDI,-DI,14";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the PDI
$command = "PDI,+DI,14";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the ADX
$command = "ADX,ADX,14";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set MDI plot style
$command = "INDICATOR_PLOT_STYLE,-DI,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set MDI color
$command = "INDICATOR_PLOT_COLOR,ALL,-DI,red";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the MDI
$command = "INDICATOR_PLOT,-DI,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set PDI plot style
$command = "INDICATOR_PLOT_STYLE,+DI,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set PDI color
$command = "INDICATOR_PLOT_COLOR,ALL,+DI,green";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the PDI
$command = "INDICATOR_PLOT,+DI,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set ADX plot style
$command = "INDICATOR_PLOT_STYLE,ADX,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set ADX color
$command = "INDICATOR_PLOT_COLOR,ALL,ADX,yellow";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the ADX
$command = "INDICATOR_PLOT,ADX,2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
