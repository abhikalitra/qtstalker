# creates a new indicator

$|=1;

# display new dialog
$command = "INDICATOR_NEW_DIALOG";
print STDOUT $command;
$name = <STDIN>; chomp($name); if ($name eq "ERROR") {print STDERR $command; exit; }

# display edit indicator dialog
$command = "INDICATOR_EDIT_DIALOG,$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create the indicator
$command = "PLOT_NEW,$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
