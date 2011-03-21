# select symbols from the symbol dialog to delete permanantly from the database

$|=1;

# get the current selected symbols from the chart panel
$command = "PLUGIN=SETTINGS_LOAD,KEY=chart_panel_selected";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the selected symbols string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SETTINGS_LOAD_DATA";
print STDOUT $command;
$symbols = <STDIN>; chomp($symbols); if ($symbols eq "ERROR") {print STDERR $command; exit; }

# display delete confirm dialog
$command = "PLUGIN=CONFIRM_DIALOG,MESSAGE=Permanantly delete selected symbols from the database?";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# display the symbol search dialog to return symbols
$command = "PLUGIN=QUOTE_DATABASE_DELETE,NAME=$symbols";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the chart panel
$command = "PLUGIN=CHART_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
