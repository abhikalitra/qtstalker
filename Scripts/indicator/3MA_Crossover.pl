# qtstalker 3MA_Crossover (OHLC Bars, 10 period SMA, 20 period SMA, 50 period SMA) indicator

$barsName = 'Bars';

$ma1Name = '10MA';
$ma1Style = 'Line';
$ma1Color = 'red';
$ma1Period = 10;

$ma2Name = '20MA';
$ma2Style = 'Line';
$ma2Color = 'yellow';
$ma2Period = 20;

$ma3Name = '50MA';
$ma3Style = 'Line';
$ma3Color = 'blue';
$ma3Period = 50;

########################################################################

$|++;

$command = "BARS,BARS,$barsName,green,red,blue";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,$barsName,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "MA,EMA,$ma1Name,$barsName,$ma1Period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$ma1Name,$ma1Style,$ma1Color,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "MA,EMA,$ma2Name,$barsName,$ma2Period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$ma2Name,$ma2Style,$ma2Color,2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "MA,EMA,$ma3Name,$barsName,$ma3Period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$ma3Name,$ma3Style,$ma3Color,3";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
