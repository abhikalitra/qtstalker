# qtstalker FAST STOCHASTIC indicator

$fastkName = 'FASTK';
$fastkStyle = 'Line';
$fastkColor = 'red';
$fastkPeriod = 5;

$fastdName = 'FASTD';
$fastdStyle = 'Line';
$fastdColor = 'yellow';
$fastdPeriod = 3;

$maType = 'EMA';

$refUpColor = 'red';
$refUp = 80;

$refDownColor = 'green';
$refDown = 20;

###################################################################

$|++;

$command = "PLUGIN=STOCH_FAST,NAME_FASTK=$fastkName,NAME_FASTD=$fastdName,PERIOD_FASTK=$fastkPeriod,PERIOD_FASTD=$fastdPeriod,MA_TYPE_FASTD=$maType";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE,TYPE=RO,PRICE=$refDown,COLOR=$refDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE,TYPE=RO,PRICE=$refUp,COLOR=$refUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$fastkName,STYLE=$fastkStyle,COLOR=$fastkColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$fastdName,STYLE=$fastdStyle,COLOR=$fastdColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
