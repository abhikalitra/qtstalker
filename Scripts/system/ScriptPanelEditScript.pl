# displays the edit script dialog

$|=1;

# get current scripts from database
$command = "SCRIPT_DATABASE,SCRIPTS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# display the selection dialog
$command = "SELECT_DIALOG,Script,$rc";
print STDOUT $command;
$script = <STDIN>; chomp($script); if ($script eq "ERROR") {print STDERR $command; exit; }

# get script parms
$command = "SCRIPT_DATABASE,LOAD,$script";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# display the edit script dialog
$command = "SCRIPT_EDIT_DIALOG,$script,$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# save the script to the database
$command = "SCRIPT_DATABASE,SAVE,$script,$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
