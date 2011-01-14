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

$command = "STOCH_SLOW,$slowkName,$slowdName,$fastkPeriod,$slowkPeriod,$maType,$slowdName,$maType";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,$refDown,$refDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,$refUp,$refUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$slowkName,$slowkStyle,$slowkColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$slowdName,$slowdStyle,$slowdColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
