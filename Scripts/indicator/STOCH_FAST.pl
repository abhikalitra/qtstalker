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

$command = "STOCH_FAST,$fastkName,$fastdName,$fastkPeriod,$fastdPeriod,$maType";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,$refDown,$refDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,$refUp,$refUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$fastkName,$fastkStyle,$fastkColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$fastdName,$fastdStyle,$fastdColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
