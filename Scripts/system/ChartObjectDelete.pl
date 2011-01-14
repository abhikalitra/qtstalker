# deletes a chart object

$|=1;

# get chart object id
$command = "SETTINGS,LOAD,chart_object_delete_id";
print STDOUT $command;
$name = <STDIN>; chomp($name); if ($name eq "ERROR") {print STDERR $command; exit; }

# confirm delete
$command = "CONFIRM_DIALOG,Confirm chart object delete";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# delete chart object from database
$command = "CHART_OBJECT_DATABASE,DELETE,$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update the app
$command = "CHART_OBJECT_DELETE,$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
