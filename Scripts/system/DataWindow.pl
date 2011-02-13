# data window dialog

$|=1;

# create the data window
$command = "PLUGIN=DATA_WINDOW";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
