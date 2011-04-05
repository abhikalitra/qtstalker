# creates a new indicator

$|=1;

# get current indicators from database
$command = "PLUGIN=INDICATOR_DATABASE,METHOD=LIST";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the indicators string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_DATABASE_INDICATORS";
print STDOUT $command;
$indicators = <STDIN>; chomp($indicators); if ($indicators eq "ERROR") {print STDERR $command; exit; }

# display new dialog
$command = "PLUGIN=NEW_DIALOG,TITLE=indicator name,ITEMS=$indicators";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the indicator string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=NEW_DIALOG_NAME";
print STDOUT $command;
$indicator = <STDIN>; chomp($indicator); if ($indicator eq "ERROR") {print STDERR $command; exit; }

# display edit indicator dialog
$command = "PLUGIN=INDICATOR_EDIT_DIALOG,NAME=$indicator";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create the indicator
$command = "PLUGIN=PLOT_NEW,NAME=$indicator";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
