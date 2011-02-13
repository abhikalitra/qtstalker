# crosshairs status has changed, update charts with new status

$|=1;

# get the current crosshairs status
$command = "PLUGIN=SETTINGS_LOAD,KEY=crosshairs";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the crosshairs string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SETTINGS_LOAD_DATA";
print STDOUT $command;
$status = <STDIN>; chomp($status); if ($status eq "ERROR") {print STDERR $command; exit; }

# set default if needed
if ($status eq "") { $status = "0"; }

# change grid status
if ($status eq "0") { $status = "1"; } else { $status = "0"; }

# save status
$command = "PLUGIN=SETTINGS_SAVE,KEY=crosshairs,DATA=$status";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update charts with new crosshairs status
$command = "PLUGIN=PLOT_CROSSHAIRS,STATUS=$status";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
