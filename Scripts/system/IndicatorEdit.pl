# updates indicator settings

$|=1;

# get the current selected indicator
$command = "PLUGIN=SETTINGS_LOAD,KEY=edit_indicator_selected";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the selected symbols string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SETTINGS_LOAD_DATA";
print STDOUT $command;
$indicator = <STDIN>; chomp($indicator); if ($indicator eq "ERROR") {print STDERR $command; exit; }

# display edit indicator dialog
$command = "PLUGIN=INDICATOR_EDIT_DIALOG,NAME=$indicator";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update indicator with new settings
$command = "PLUGIN=PLOT_UPDATE,NAME=$indicator";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
