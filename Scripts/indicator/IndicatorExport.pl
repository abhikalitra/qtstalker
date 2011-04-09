# exports indicator from the database to an ascii file

$|=1;

# get indicators from database
$command = "PLUGIN=INDICATOR_DATABASE,METHOD=LIST";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the indicators string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_DATABASE_INDICATORS";
print STDOUT $command;
$indicators = <STDIN>; chomp($indicators); if ($indicators eq "ERROR") {print STDERR $command; exit; }

# display the selection dialog
$command = "PLUGIN=SELECT_DIALOG,MODE=0,TITLE=Indicator,ITEMS=$indicators";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the selected indicators string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SELECT_DIALOG_SELECTED";
print STDOUT $command;
$indicators = <STDIN>; chomp($indicators); if ($indicators eq "ERROR") {print STDERR $command; exit; }

# display the file selection dialog 
$command = "PLUGIN=FILE_DIALOG,MODE=2,TITLE=Select Directory to Export Indicators";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the directory
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=FILE_DIALOG_FILE";
print STDOUT $command;
$dir = <STDIN>; chomp($dir); if ($dir eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_DATABASE,METHOD=DUMP,NAME=$indicators,DIRECTORY=$dir";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
