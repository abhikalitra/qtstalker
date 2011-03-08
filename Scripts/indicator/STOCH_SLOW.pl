# qtstalker SLOW STOCHASTIC indicator

$slowkName = 'SLOWK';
$slowkStyle = 'Line';
$slowkColor = 'red';
$slowkPeriod = 3;

$slowdName = 'SLOWD';
$slowdStyle = 'Line';
$slowdColor = 'yellow';
$slowdPeriod = 3;

$maType = 'EMA';
$fastkPeriod = 5;

$refUpColor = 'red';
$refUp = 80;

$refDownColor = 'green';
$refDown = 20;

###################################################################

$|++;

$command = "PLUGIN=STOCH_SLOW,NAME_SLOWK=$slowkName,NAME_SLOWD=$slowdName,PERIOD_FASTK=$fastkPeriod,PERIOD_SLOWK=$slowkPeriod,MA_TYPE_SLOWK=$maType,PERIOD_SLOWD=$slowdPeriod,MA_TYPE_SLOWD=$maType";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE,TYPE=RO,PRICE=$refDown,COLOR=$refDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE,TYPE=RO,PRICE=$refUp,COLOR=$refUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$slowkName,STYLE=$slowkStyle,COLOR=$slowkColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$slowdName,STYLE=$slowdStyle,COLOR=$slowdColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
