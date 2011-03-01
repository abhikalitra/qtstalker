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

$period = 20;
$maType = 'SMA';
$udev = 2;
$ddev = 2;

###################################################################

$|++;

$command = "PLUGIN=OHLC,NAME=$barsName,COLOR_UP=green,COLOR_DOWN=red,COLOR_NEUTRAL=blue";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT,NAME=$barsName,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=BBANDS,INPUT=$barsName,NAME_UPPER=$bbUpName,NAME_MIDDLE=$bbMidName,NAME_LOWER=$bbDownName,PERIOD=$period,MA_TYPE=$maType,DEV_UP=$udev,DEV_DOWN=$ddev";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$bbUpName,STYLE=$bbUpStyle,COLOR=$bbUpColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$bbMidName,STYLE=$bbMidStyle,COLOR=$bbMidColor,Z=2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$bbDownName,STYLE=$bbDownStyle,COLOR=$bbDownColor,Z=3";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
