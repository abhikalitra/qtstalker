# crosshairs color dialog

$|=1;

# get the current crosshairs color
$command = "SETTINGS_LOAD,crosshairs_color";
print STDOUT $command;
$color = <STDIN>; chomp($color); if ($color eq "ERROR") {print STDERR $command; exit; }

# set default if needed
if ($color eq "") { $color = "white"; }

# color dialog
$command = "COLOR_DIALOG,$color";
print STDOUT $command;
$color = <STDIN>; chomp($color); if ($color eq "ERROR") {print STDERR $font; exit; }

# save color
$command = "SETTINGS_SAVE,crosshairs_color,$color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update charts with new crosshairs color
$command = "PLOT_CROSSHAIRS_COLOR,$color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
