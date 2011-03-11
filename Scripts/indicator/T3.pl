# qtstalker T3 indicator

$barsName = 'Bars';

$t3Name = 'T3';
$t3Style = 'Line';
$t3Color = 'yellow';
$t3Period = 5;
$vfactor = 0.7;

$method = 'Long';

$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';

###################################################################

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
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT,NAME=$barsName,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=T3,NAME=$t3Name,INPUT=$barsName,PERIOD=$t3Period,VFACTOR=$vfactor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$t3Name,STYLE=$t3Style,COLOR=$t3Color,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
