# creates a new script and records it in the script database
# then opens the script edit dialog

$|=1;

$command = "SCRIPT_SELECT_DIALOG";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

my @items = split(',', $rc);

$command = "SCRIPT_EDIT_DIALOG,$items[0]";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
