# qtstalker BBANDS indicator

$barsName = 'Bars';

$bbUpName = 'BB+';
$bbUpStyle = 'Line';
$bbUpColor = 'red';

$bbMidName = 'BB';
$bbMidStyle = 'Line';
$bbMidColor = 'yellow';

$bbDownName = 'BB-';
$bbDownStyle = 'Line';
$bbDownColor = 'green';

$period = 14;
$maType = 'EMA';
$udev = 1;
$ddev = 1;

###################################################################

$|++;

$command = "BARS,BARS,$barsName,green,red,blue";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,$barsName,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "BBANDS,$barsName,$bbUpName,$bbMidName,$bbDownName,$period,$maType,$udev,$ddev";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$bbUpName,$bbUpStyle,$bbUpColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$bbMidName,$bbMidStyle,$bbMidColor,2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$bbDownName,$bbDownStyle,$bbDownColor,3";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
