# grid status has changed, update charts with new status

$|=1;

# get the current grid status
$command = "SETTINGS_LOAD,grid";
print STDOUT $command;
$status = <STDIN>; chomp($status); if ($status eq "ERROR") {print STDERR $command; exit; }

# change grid status
if ($status eq "0") { $status = "1"; } else { $status = "0"; }

# save status
$command = "SETTINGS_SAVE,grid,$status";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update charts with new grid status
$command = "PLOT_GRID,$status";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
