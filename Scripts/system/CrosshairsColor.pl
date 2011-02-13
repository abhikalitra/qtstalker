# crosshairs color dialog

$|=1;

# get the current crosshairs color
$command = "PLUGIN=SETTINGS_LOAD,KEY=crosshairs_color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the color string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SETTINGS_LOAD_DATA";
print STDOUT $command;
$color = <STDIN>; chomp($color); if ($color eq "ERROR") {print STDERR $command; exit; }

# set default if needed
if ($color eq "") { $color = "white"; }

# color dialog
$command = "PLUGIN=COLOR_DIALOG,COLOR=$color";
print STDOUT $command;
$color = <STDIN>; chomp($color); if ($color eq "ERROR") {print STDERR $font; exit; }

# save color
$command = "PLUGIN=SETTINGS_SAVE,KEY=crosshairs_color,DATA=$color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update charts with new crosshairs color
$command = "PLUGIN=PLOT_CROSSHAIRS_COLOR,COLOR=$color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
