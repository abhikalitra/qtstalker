# qtstalker MACD indicator

$input = 'c';

$macdName = 'MACD';
$macdStyle = 'Line';
$macdColor = 'red';

$sigName = 'MACD_SIG';
$sigStyle = 'Line';
$sigColor = 'yellow';
$sigPeriod = 9;

$histName = 'MACD_HIST';
$histStyle = 'HistogramBar';
$histColor = 'blue';

$fastPeriod = 12;
$slowPeriod = 26;
$maType = 'EMA';

###################################################################

$|++;

$command = "PLUGIN=DOHLCVI,METHOD=C,NAME=$input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MACD,INPUT=$input,NAME_MACD=$macdName,NAME_SIGNAL=$sigName,NAME_HIST=$histName,PERIOD_FAST=$fastPeriod,MA_TYPE_FAST=$maType,PERIOD_SLOW=$slowPeriod,MA_TYPE_SLOW=$maType,PERIOD_SIGNAL=$sigPeriod,MA_TYPE_SIGNAL=$maType";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$histName,STYLE=$histStyle,COLOR=$histColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$macdName,STYLE=$macdStyle,COLOR=$macdColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$sigName,STYLE=$sigStyle,COLOR=$sigColor,Z=2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
