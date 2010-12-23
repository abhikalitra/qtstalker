# creates a new script and records it in the script database
# then opens the script edit dialog

$|=1;

# get current groups from database
$command = "GROUP_DATABASE,GROUPS";
print STDOUT $command;
$groups = <STDIN>; chomp($groups); if ($groups eq "ERROR") {print STDERR $command; exit; }

# display new group dialog
$command = "NEW_DIALOG,Group,$groups";
print STDOUT $command;
$group = <STDIN>; chomp($group); if ($group eq "ERROR") {print STDERR $command; exit; }

# display group edit dialog
$command = "GROUP_EDIT_DIALOG,$group";
print STDOUT $command;
$items = <STDIN>; chomp($items); if ($items eq "ERROR") {print STDERR $command; print STDERR $items; exit; }

# save the new group
$command = "GROUP_DATABASE,SAVE,$group,$items";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the group panel list
$command = "GROUP_PANEL,REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
