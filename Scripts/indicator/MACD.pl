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
$histStyle = 'Histogram';
$histColor = 'blue';

$fastPeriod = 12;
$slowPeriod = 26;
$maType = 'EMA';

###################################################################

$|++;

$command = "BARS,Close,$input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "MACD,$input,$macdName,$sigName,$histName,$fastPeriod,$maType,$slowPeriod,$maType,$sigPeriod,$maType";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$histName,$histStyle,$histColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$macdName,$macdStyle,$macdColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$sigName,$sigStyle,$sigColor,2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
