# plot font dialog

$|=1;
  
# get the current font
$command = "SETTINGS,LOAD,plot_font";
print STDOUT $command;
$font = <STDIN>; chomp($font); if ($font eq "ERROR") {print STDERR $command; exit; }

# font dialog
$command = "FONT_DIALOG,$font";
print STDOUT $command;
$font = <STDIN>; chomp($font); if ($font eq "ERROR") {print STDERR $font; exit; }

# save font
$command = "SETTINGS,SAVE,plot_font,$font";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update charts with new font
$command = "PLOT,FONT,$font";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
