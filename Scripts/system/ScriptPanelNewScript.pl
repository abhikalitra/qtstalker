# creates a new script and records it in the script database

$|=1;

# get current scripts from database
$command = "SCRIPT_DATABASE,SCRIPTS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# display new script dialog
$command = "NEW_DIALOG,Script,$rc";
print STDOUT $command;
$script = <STDIN>; chomp($script); if ($script eq "ERROR") {print STDERR $command; exit; }

# display script edit dialog
$command = "SCRIPT_EDIT_DIALOG,$script";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
