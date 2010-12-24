# creates a new script and records it in the script database
# then opens the script edit dialog

$|=1;

# get current groups from database
$command = "GROUP_DATABASE,GROUPS";
print STDOUT $command;
$groups = <STDIN>; chomp($groups); if ($groups eq "ERROR") {print STDERR $command; exit; }

$command = "SELECT_DIALOG,1,Group,$groups";
print STDOUT $command;
$group = <STDIN>; chomp($group); if ($group eq "ERROR") {print STDERR $command; exit; }

# get group contents
$command = "GROUP_DATABASE,LOAD,$group";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

$command = "GROUP_EDIT_DIALOG,$group,$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get current groups from database
$command = "GROUP_DATABASE,SAVE_ALL,$group,$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the group panel list
$command = "GROUP_PANEL,REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
