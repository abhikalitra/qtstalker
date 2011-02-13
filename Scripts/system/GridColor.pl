# plot grid color dialog

$|=1;

# get the current grid color
$command = "PLUGIN=SETTINGS_LOAD,KEY=grid_color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the grid color string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SETTINGS_LOAD_DATA";
print STDOUT $command;
$color = <STDIN>; chomp($color); if ($color eq "ERROR") {print STDERR $command; exit; }

# set default if needed
if ($color eq "") { $color = "#626262"; }

# color dialog
$command = "PLUGIN=COLOR_DIALOG,COLOR=$color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $font; exit; }

# get the color string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=COLOR_DIALOG_COLOR";
print STDOUT $command;
$color = <STDIN>; chomp($color); if ($color eq "ERROR") {print STDERR $command; exit; }

# save color
$command = "PLUGIN=SETTINGS_SAVE,KEY=grid_color,DATA=$color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update charts with new background color
$command = "PLUGIN=PLOT_GRID_COLOR,COLOR=$color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
