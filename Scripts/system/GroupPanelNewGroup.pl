# creates a new script and records it in the script database
# then opens the script edit dialog

$|=1;

# get current groups from database
$command = "PLUGIN=GROUP_DATABASE_LIST";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the groups string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=GROUP_DATABASE_GROUPS";
print STDOUT $command;
$groups = <STDIN>; chomp($groups); if ($groups eq "ERROR") {print STDERR $command; exit; }

# display new group dialog
$command = "PLUGIN=NEW_DIALOG,TITLE=Group,ITEMS=$groups";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the group string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=NEW_DIALOG_NAME";
print STDOUT $command;
$group = <STDIN>; chomp($group); if ($group eq "ERROR") {print STDERR $command; exit; }

# display group edit dialog
$command = "PLUGIN=GROUP_EDIT_DIALOG,NAME=$group";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; print STDERR $items; exit; }

# refresh the group panel list
$command = "PLUGIN=GROUP_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
