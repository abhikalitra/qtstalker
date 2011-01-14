# qtstalker AD indicator

$name = 'AD';
$style = 'Line';
$color = 'red';

###################################################################

$|++;

$command = "AD,$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$name,$style,$color,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
