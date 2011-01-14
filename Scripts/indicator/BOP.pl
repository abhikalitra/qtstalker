# qtstalker BOP indicator

$name = 'BOP';
$style = 'Line';
$color = 'red';

###################################################################

$|++;

$command = "BOP,$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$name,$style,$color,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
