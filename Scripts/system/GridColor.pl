# plot grid color dialog

$|=1;

# get the current grid color
$command = "SETTINGS_LOAD,grid_color";
print STDOUT $command;
$color = <STDIN>; chomp($color); if ($color eq "ERROR") {print STDERR $command; exit; }

# set default if needed
if ($color eq "") { $color = "#626262"; }

# color dialog
$command = "COLOR_DIALOG,$color";
print STDOUT $command;
$color = <STDIN>; chomp($color); if ($color eq "ERROR") {print STDERR $font; exit; }

# save color
$command = "SETTINGS_SAVE,grid_color,$color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update charts with new background color
$command = "PLOT_GRID_COLOR,$color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
