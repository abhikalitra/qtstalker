# plot background color dialog

$|=1;

# get the current background color
$command = "SETTINGS,LOAD,plot_background_color";
print STDOUT $command;
$color = <STDIN>; chomp($color); if ($color eq "ERROR") {print STDERR $command; exit; }

# set default if needed
if ($color eq "") { $color = "black"; }

# color dialog
$command = "COLOR_DIALOG,$color";
print STDOUT $command;
$color = <STDIN>; chomp($color); if ($color eq "ERROR") {print STDERR $color; exit; }

# save color
$command = "SETTINGS,SAVE,plot_background_color,$color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update charts with new background color
$command = "PLOT,BACKGROUND_COLOR,$color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
