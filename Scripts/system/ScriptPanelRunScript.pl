# deletes scripts from the database

$|=1;

# get current scripts from database
$command = "SCRIPT_DATABASE,SCRIPTS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# display the selection dialog
$command = "SELECT_DIALOG,0,Script,$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

$command = "SCRIPT_PANEL,RUN,$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
