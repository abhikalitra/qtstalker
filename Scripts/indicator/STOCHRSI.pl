# qtstalker STOCHASTIC RSI indicator

$input = 'c';

$stochName = 'STOCHRSI';
$stochStyle = 'Line';
$stochColor = 'yellow';
$stochPeriod = 14;

$refUpColor = 'red';
$refUp = 80;

$refDownColor = 'green';
$refDown = 20;

###################################################################

$|++;

$command = "BARS,Close,$input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "STOCH_RSI,$stochName,$input,$stochPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,$refDown,$refDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,$refUp,$refUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$stcohName,$stochStyle,$stochColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
