# deletes a chart object

$|=1;

# get chart object id
$command = "PLUGIN=SETTINGS_LOAD,KEY=chart_object_delete_id";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# return the chart object id string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SETTINGS_LOAD_DATA";
print STDOUT $command;
$name = <STDIN>; chomp($name); if ($name eq "ERROR") { print STDERR $command; next; }

# confirm delete
$command = "PLUGIN=CONFIRM_DIALOG,MESSAGE=Confirm chart object delete $name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# delete chart object from database
$command = "PLUGIN=CHART_OBJECT_DATABASE_DELETE,ID=$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update the app
$command = "PLUGIN=CHART_OBJECT_DELETE,LIST=$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
