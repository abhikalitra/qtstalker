# deletes groups from the database

$|=1;

# get current groups from database
$command = "PLUGIN=GROUP_DATABASE_LIST";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the groups string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=GROUP_DATABASE_LIST_GROUPS";
print STDOUT $command;
$groups = <STDIN>; chomp($groups); if ($groups eq "ERROR") {print STDERR $command; exit; }

# display the group delete dialog
$command = "PLUGIN=SELECT_DIALOG,MODE=0,TITLE=Group,ITEMS=$groups";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the group string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SELECT_DIALOG_SELECTED";
print STDOUT $command;
$groups = <STDIN>; chomp($groups); if ($groups eq "ERROR") {print STDERR $command; exit; }

$command = "PLUGIN=GROUP_DATABASE_DELETE,NAME=$groups";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the group panel list
$command = "PLUGIN=GROUP_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
