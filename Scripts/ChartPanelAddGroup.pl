# adds symbols selected from the chart panel to a group

$|=1;

# get current groups from database
$command = "GROUP_DATABASE,GROUPS";
print STDOUT $command;
$groups = <STDIN>; chomp($groups); if ($groups eq "ERROR") {print STDERR $command; exit; }

# display the select group dialog
$command = "SELECT_DIALOG,Group,$groups";
print STDOUT $command;
$group = <STDIN>; chomp($group); if ($group eq "ERROR") {print STDERR $command; exit; }

# get symbols selected from the chart panel
$command = "CHART_PANEL,SELECTED";
print STDOUT $command;
$symbols = <STDIN>; chomp($symbols); if ($symbols eq "ERROR") {print STDERR $command; exit; }

# save symbols to group
$command = "GROUP_DATABASE,SAVE,$group,$symbols";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the chart panel
$command = "CHART_PANEL,REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
