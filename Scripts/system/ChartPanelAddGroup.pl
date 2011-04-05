# adds symbols selected from the chart panel to a group

$|=1;

# get current groups from database
$command = "PLUGIN=GROUP_DATABASE,METHOD=LIST";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the groups string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=GROUP_DATABASE_GROUPS";
print STDOUT $command;
$groups = <STDIN>; chomp($groups); if ($groups eq "ERROR") {print STDERR $command; exit; }

# display the select group dialog
$command = "PLUGIN=SELECT_DIALOG,MODE=1,TITLE=Group,ITEMS=$groups";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the selected group to modify
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SELECT_DIALOG_SELECTED";
print STDOUT $command;
$group = <STDIN>; chomp($group); if ($group eq "ERROR") {print STDERR $command; exit; }

# get the current selected symbols from the chart panel
$command = "PLUGIN=SETTINGS,METHOD=LOAD,KEY=chart_panel_selected";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the selected symbols string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SETTINGS_DATA";
print STDOUT $command;
$symbols = <STDIN>; chomp($symbols); if ($symbols eq "ERROR") {print STDERR $command; exit; }

# save symbols to group
$command = "PLUGIN=GROUP_DATABASE,METHOD=SAVE,NAME=$group,ITEMS=$symbols";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the group panel list
$command = "PLUGIN=CONTROL_PANEL,METHOD=GROUP_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
