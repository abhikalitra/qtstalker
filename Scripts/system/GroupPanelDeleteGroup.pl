# deletes groups from the database

$|=1;

# get current groups from database
$command = "GROUP_DATABASE,GROUPS";
print STDOUT $command;
$groups = <STDIN>; chomp($groups); if ($groups eq "ERROR") {print STDERR $command; exit; }

# display the group delete dialog
$command = "SELECT_DIALOG,Group,$groups";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

$command = "GROUP_DATABASE,DELETE,$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the group panel list
$command = "GROUP_PANEL,REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
