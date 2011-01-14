# qtstalker FI indicator

$name = 'FI';
$style = 'Histogram Bar';
$color = 'yellow';
$period = 2;
$maType = 'EMA';

###################################################################

$|++;

$command = "FI,$name,$period,$maType";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$name,$style,$color,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
