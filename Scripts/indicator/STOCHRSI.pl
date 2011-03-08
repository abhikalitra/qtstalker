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

$command = "PLUGIN=CLOSE,NAME=$input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=STOCH_RSI,NAME=$stochName,INPUT=$input,PERIOD=$stochPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE,TYPE=RO,PRICE=$refDown,COLOR=$refDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE,TYPE=RO,PRICE=$refUp,COLOR=$refUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$stochName,STYLE=$stochStyle,COLOR=$stochColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
