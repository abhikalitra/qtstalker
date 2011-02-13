# configure dialog

$|=1;

# display the configure dialog
$command = "PLUGIN=CONFIGURE_DIALOG";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
