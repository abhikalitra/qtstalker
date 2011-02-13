# adds selected symbols to a group from the group panel

$|=1;

# get current groups from database
$command = "PLUGIN=GROUP_DATABASE_LIST";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the groups string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=GROUP_DATABASE_GROUPS";
print STDOUT $command;
$groups = <STDIN>; chomp($groups); if ($groups eq "ERROR") {print STDERR $command; exit; }

$command = "PLUGIN=SELECT_DIALOG,MODE=1,TITLE=Group,ITEMS=$groups";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the group string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SELECT_DIALOG_SELECTED";
print STDOUT $command;
$group = <STDIN>; chomp($group); if ($group eq "ERROR") {print STDERR $command; exit; }

$command = "PLUGIN=SYMBOL_DIALOG,FLAG=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the symbols string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SYMBOL_DIALOG_SYMBOLS";
print STDOUT $command;
$symbols = <STDIN>; chomp($symbols); if ($symbols eq "ERROR") {print STDERR $command; exit; }

$command = "PLUGIN=GROUP_DATABASE_SAVE,NAME=$group,ITEMS=$symbols";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the group panel list
$command = "PLUGIN=GROUP_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
