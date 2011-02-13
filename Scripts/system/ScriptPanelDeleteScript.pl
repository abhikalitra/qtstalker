# deletes scripts from the database

$|=1;

# get current scripts from database
$command = "PLUGIN=SCRIPT_DATABASE,METHOD=SCRIPTS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the scripts string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SCRIPT_DATABASE_SCRIPTS";
print STDOUT $command;
$scripts = <STDIN>; chomp($scripts); if ($scripts eq "ERROR") {print STDERR $command; exit; }

# display the selection dialog
$command = "PLUGIN=SELECT_DIALOG,MODE=0,TITLE=Script,ITEMS=$scripts";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the selected scripts string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SELECT_DIALOG_SELECTED";
print STDOUT $command;
$scripts = <STDIN>; chomp($scripts); if ($scripts eq "ERROR") {print STDERR $command; exit; }

$command = "PLUGIN=SCRIPT_DATABASE,METHOD=DELETE,NAME=$scripts";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
