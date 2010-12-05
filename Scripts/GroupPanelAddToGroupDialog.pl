# creates a new script and records it in the script database
# then opens the script edit dialog

$|=1;

$command = "GROUP_SELECT_DIALOG";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

my @group = split(',', $rc);

$command = "SYMBOL_DIALOG";
print STDOUT $command;
$symbols = <STDIN>; chomp($symbols); if ($symbols eq "ERROR") {print STDERR $command; exit; }

$command = "GROUP_ADD_ITEMS,$group[0],$symbols";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the group panel list
$command = "MESSAGE,REFRESH_GROUP_PANEL";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
