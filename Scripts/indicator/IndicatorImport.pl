# imports indicator ascii files previously exported by qtstalker

$|=1;

# display the file selection dialog 
$command = "PLUGIN=FILE_DIALOG,MODE=1,TITLE=Select Indicator Files";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the files
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=FILE_DIALOG_FILE";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_DATABASE,METHOD=IMPORT,NAME=$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
