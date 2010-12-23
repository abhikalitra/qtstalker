# adds selected symbols to a group from the group panel

$|=1;

# get current groups from database
$command = "GROUP_DATABASE,GROUPS";
print STDOUT $command;
$groups = <STDIN>; chomp($groups); if ($groups eq "ERROR") {print STDERR $command; exit; }

$command = "SELECT_DIALOG,Group,$groups";
print STDOUT $command;
$group = <STDIN>; chomp($group); if ($group eq "ERROR") {print STDERR $command; exit; }

$command = "SYMBOL_DIALOG,0";
print STDOUT $command;
$symbols = <STDIN>; chomp($symbols); if ($symbols eq "ERROR") {print STDERR $command; exit; }

$command = "GROUP_DATABASE,SAVE,$group,$symbols";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the group panel list
$command = "GROUP_PANEL,REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
