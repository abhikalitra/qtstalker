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

$command = "PLUGIN=OHLC,NAME=$barsName,COLOR_UP=green,COLOR_DOWN=red,COLOR_NEUTRAL=blue";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT,NAME=$barsName,Z=0";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MA,METHOD=EMA,NAME=$ma1Name,INPUT=$barsName,PERIOD=$ma1Period";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$ma1Name,STYLE=$ma1Style,COLOR=$ma1Color,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MA,METHOD=EMA,NAME=$ma2Name,INPUT=$barsName,PERIOD=$ma2Period";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$ma2Name,STYLE=$ma2Style,COLOR=$ma2Color,Z=2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
