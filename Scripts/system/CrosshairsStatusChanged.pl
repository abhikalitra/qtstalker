# crosshairs status has changed, update charts with new status

$|=1;

# get the current crosshairs status
$command = "SETTINGS,LOAD,crosshairs";
print STDOUT $command;
$status = <STDIN>; chomp($status); if ($status eq "ERROR") {print STDERR $command; exit; }

# set default if needed
if ($status eq "") { $status = "0"; }

# change grid status
if ($status eq "0") { $status = "1"; } else { $status = "0"; }

# save status
$command = "SETTINGS,SAVE,crosshairs,$status";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update charts with new crosshairs status
$command = "PLOT,CROSSHAIRS,$status";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
