# qtstalker 2MA_Crossover (OHLC Bars, 50 period SMA, 200 period SMA) indicator

$barsName = 'Bars';

$ma1Name = '50MA';
$ma1Style = 'Line';
$ma1Color = 'yellow';
$ma1Period = 50;

$ma2Name = '200MA';
$ma2Style = 'Line';
$ma2Color = 'blue';
$ma2Period = 200;

########################################################################

$|++;

$command = "BARS,BARS,$barsName,green,red,blue";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,$barsName,0";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "MA,EMA,$ma1Name,$barsName,$ma1Period";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$ma1Name,$ma1Style,$ma1Color,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "MA,EMA,$ma2Name,$barsName,$ma2Period";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$ma2Name,$ma2Style,$ma2Color,2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
