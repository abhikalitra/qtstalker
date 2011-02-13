# qtstalker RSI indicator

$input = 'c';

$rsiName = 'RSI';
$rsiStyle = 'Line';
$rsiColor = 'red';
$rsiPeriod = 14;

$ma1Name = 'RSI_MA';
$ma1Period = '9';
$ma1Color = 'yellow';
$ma1Style = 'Line';

$refUpColor = 'red';
$refUp = 70;
$refDownColor = 'green';
$refDown = 30;

###################################################################

$|++;

$command = "PLUGIN=CLOSE,NAME=$input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=RSI,NAME=$rsiName,INPUT=$input,PERIOD=$rsiPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE_TEMP,PRICE=$refDown,COLOR=$refDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE_TEMP,PRICE=$refUp,COLOR=$refUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$rsiName,STYLE=$rsiStyle,COLOR=$rsiColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MA,METHOD=EMA,NAME=$ma1Name,INPUT=$rsiName,PERIOD=$ma1Period";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$ma1Name,STYLE=$ma1Style,COLOR=$ma1Color,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
