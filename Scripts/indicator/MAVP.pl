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

$command = "PLUGIN=OHLC,NAME=$barsName,COLOR_UP=green,COLOR_DOWN=red,COLOR_NEUTRAL=blue";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT,NAME=$barsName,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MAVP,NAME=$mavpName,INPUT=$barsName,INPUT2=$barsName,PERIOD_MIN=$minPeriod,PERIOD_MAX=$maxPeriod,MA_TYPE=$maType";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$mavpName,STYLE=$mavpStyle,COLOR=$mavpColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
