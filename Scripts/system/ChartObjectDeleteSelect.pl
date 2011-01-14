# deletes an indicator

$|=1;

# get indicator to select from
$command = "SETTINGS,LOAD,chart_object_delete_data";
print STDOUT $command;
$indicator = <STDIN>; chomp($indicator); if ($indicator eq "ERROR") {print STDERR $command; exit; }

# get current chart objects from database
$command = "CHART_OBJECT_DATABASE,IDS,$indicator";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# display select dialog
$command = "SELECT_DIALOG,0,Chart Objects,$rc";
print STDOUT $command;
$list = <STDIN>; chomp($list); if ($list eq "ERROR") {print STDERR $command; exit; }

# confirm delete
$command = "CONFIRM_DIALOG,Confirm chart object delete";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# delete chart object from database
$command = "CHART_OBJECT_DATABASE,DELETE,$list";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update the app
$command = "CHART_OBJECT_DELETE,$list";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
