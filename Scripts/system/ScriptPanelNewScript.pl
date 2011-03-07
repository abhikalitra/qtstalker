# creates a new script and records it in the script database

$|=1;

# get current scripts from database
$command = "PLUGIN=SCRIPT_DATABASE_LIST";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the scripts string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SCRIPT_DATABASE_LIST_SCRIPTS";
print STDOUT $command;
$scripts = <STDIN>; chomp($scripts); if ($scripts eq "ERROR") {print STDERR $command; exit; }

# display new script dialog
$command = "PLUGIN=NEW_DIALOG,TITLE=Script,ITEMS=$scripts";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the name string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=NEW_DIALOG_NAME";
print STDOUT $command;
$name = <STDIN>; chomp($name); if ($name eq "ERROR") {print STDERR $command; exit; }

# display script edit dialog
$command = "PLUGIN=SCRIPT_EDIT_DIALOG,NAME=$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
