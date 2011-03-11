# qtstalker 3MA_Crossover (OHLC Bars, 10 period SMA, 20 period SMA, 50 period SMA) indicator

$barsName = 'Bars';
$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';

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

$command = "PLUGIN=OPEN,NAME=$openName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=HIGH,NAME=$highName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=LOW,NAME=$lowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CLOSE,NAME=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=OHLC,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,NAME=$barsName,COLOR_UP=green,COLOR_DOWN=red,COLOR_NEUTRAL=blue";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT,NAME=$barsName,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MA,METHOD=EMA,NAME=$ma1Name,INPUT=$barsName,PERIOD=$ma1Period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$ma1Name,STYLE=$ma1Style,COLOR=$ma1Color,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MA,METHOD=EMA,NAME=$ma2Name,INPUT=$barsName,PERIOD=$ma2Period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$ma2Name,STYLE=$ma2Style,COLOR=$ma2Color,Z=2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MA,METHOD=EMA,NAME=$ma3Name,INPUT=$barsName,PERIOD=$ma3Period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$ma3Name,STYLE=$ma3Style,COLOR=$ma3Color,Z=3";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
