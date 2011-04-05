# deletes an indicator

$|=1;

# get current indicators from database
$command = "PLUGIN=INDICATOR_DATABASE,METHOD=LIST";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the indicators string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_DATABASE_INDICATORS";
print STDOUT $command;
$indicators = <STDIN>; chomp($indicators); if ($indicators eq "ERROR") {print STDERR $command; exit; }

# display select dialog
$command = "PLUGIN=SELECT_DIALOG,MODE=0,TITLE=Indicator,ITEMS=$indicators";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the indicator string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SELECT_DIALOG_SELECTED";
print STDOUT $command;
$list = <STDIN>; chomp($list); if ($list eq "ERROR") {print STDERR $command; exit; }

# delete indicators from the database
$command = "PLUGIN=INDICATOR_DATABASE,METHOD=DELETE,NAME=$list";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# delete the charts
$command = "PLUGIN=PLOT_DELETE,NAME=$list";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
