# updates indicator settings

$|=1;

# get current indicators from database
$command = "PLUGIN=INDICATOR_DATABASE_LIST";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the indicators string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_DATABASE_LIST_INDICATORS";
print STDOUT $command;
$indicators = <STDIN>; chomp($indicators); if ($indicators eq "ERROR") {print STDERR $command; exit; }

# display select dialog
$command = "PLUGIN=SELECT_DIALOG,MODE=1,TITLE=Indicator,ITEMS=$indicators";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the indicator string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SELECT_DIALOG_SELECTED";
print STDOUT $command;
$indicator = <STDIN>; chomp($indicator); if ($indicator eq "ERROR") {print STDERR $command; exit; }

# display edit indicator dialog
$command = "PLUGIN=INDICATOR_EDIT_DIALOG,NAME=$indicator";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update indicator with new settings
$command = "PLUGIN=PLOT_UPDATE,NAME=$indicator";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
