# plot font dialog

$|=1;

# display the font dialog
$command = "DATA_WINDOW";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
