# grid status has changed, update charts with new status

$|=1;

# get the current grid status
$command = "PLUGIN=SETTINGS_LOAD,KEY=grid";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the grid string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SETTINGS_LOAD_DATA";
print STDOUT $command;
$status = <STDIN>; chomp($status); if ($status eq "ERROR") {print STDERR $command; exit; }

if ($status eq "") { $status = "1"; }

# change grid status
if ($status eq "0") { $status = "1"; } else { $status = "0"; }

# save status
$command = "PLUGIN=SETTINGS_SAVE,KEY=grid,DATA=$status";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update charts with new grid status
$command = "PLUGIN=PLOT_GRID,STATUS=$status";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
