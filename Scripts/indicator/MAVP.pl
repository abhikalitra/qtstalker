# qtstalker MAVP indicator

$barsName = 'Bars';

$mavpName = 'MAVP';
$mavpStyle = 'Line';
$mavpColor = 'red';

$minPeriod = 2;
$maxPeriod = 30;
$maType = 'EMA';

###################################################################

$|++;

$command = "BARS,BARS,$barsName,green,red,blue";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,$barsName,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "MAVP,$mavpName,$barsName,$barsName,$minPeriod,$maxPeriod,$maType";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$mavpName,$mavpStyle,$mavpColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
