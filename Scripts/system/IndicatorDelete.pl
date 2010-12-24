# deletes an indicator

$|=1;

# get current indicators from database
$command = "INDICATOR_DATABASE,INDICATORS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# display select dialog
$command = "SELECT_DIALOG,0,Indicator,$rc";
print STDOUT $command;
$list = <STDIN>; chomp($list); if ($list eq "ERROR") {print STDERR $command; exit; }

# delete indicators from the database
$command = "INDICATOR_DATABASE,DELETE,$list";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }

# delete the charts
$command = "PLOT_LAYOUT,DELETE,$list";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }
