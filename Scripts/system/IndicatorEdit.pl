# updates indicator settings

$|=1;

# get current indicators from database
$command = "INDICATOR_DATABASE,INDICATORS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# display select dialog
$command = "SELECT_DIALOG,1,Indicator,$rc";
print STDOUT $command;
$name = <STDIN>; chomp($name); if ($name eq "ERROR") {print STDERR $command; exit; }

# display edit indicator dialog
$command = "INDICATOR_EDIT_DIALOG,$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update indicator with new settings
$command = "PLOT_UPDATE,$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
