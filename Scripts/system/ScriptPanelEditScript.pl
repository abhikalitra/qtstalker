# displays the edit script dialog

$|=1;

# get current scripts from database
$command = "PLUGIN=SCRIPT_DATABASE_LIST";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the scripts string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SCRIPT_DATABASE_LIST_SCRIPTS";
print STDOUT $command;
$scripts = <STDIN>; chomp($scripts); if ($scripts eq "ERROR") {print STDERR $command; exit; }

# display the selection dialog
$command = "PLUGIN=SELECT_DIALOG,MODE=1,TITLE=Script,ITEMS=$scripts";
print STDOUT $command;
$script = <STDIN>; chomp($script); if ($script eq "ERROR") {print STDERR $command; exit; }

# get the selected scripts string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SELECT_DIALOG_SELECTED";
print STDOUT $command;
$script = <STDIN>; chomp($script); if ($script eq "ERROR") {print STDERR $command; exit; }

# display the edit script dialog
$command = "PLUGIN=SCRIPT_EDIT_DIALOG,NAME=$script";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
