# creates a new script and records it in the script database
# then opens the script edit dialog

$|=1;

$command = "SCRIPT_NEW_DIALOG";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

$command = "SCRIPT_EDIT_DIALOG,$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
