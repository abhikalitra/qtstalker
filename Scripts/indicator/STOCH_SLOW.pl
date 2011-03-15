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

$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';

###################################################################

$|++;

$command = "PLUGIN=DOHLCVI,METHOD=H,NAME=$highName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=DOHLCVI,METHOD=L,NAME=$lowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=DOHLCVI,METHOD=C,NAME=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=STOCH_SLOW,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,NAME_SLOWK=$slowkName,NAME_SLOWD=$slowdName,PERIOD_FASTK=$fastkPeriod,PERIOD_SLOWK=$slowkPeriod,MA_TYPE_SLOWK=$maType,PERIOD_SLOWD=$slowdPeriod,MA_TYPE_SLOWD=$maType";
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
