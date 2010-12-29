# creates a new indicator

$|=1;

# get current indicators from database
$command = "INDICATOR_DATABASE,INDICATORS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# display new dialog
$command = "NEW_DIALOG,Indicator,$rc";
print STDOUT $command;
$name = <STDIN>; chomp($name); if ($name eq "ERROR") {print STDERR $command; exit; }

# display edit indicator dialog
$command = "INDICATOR_EDIT_DIALOG,$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create the chart
$command = "PLOT,NEW,$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
