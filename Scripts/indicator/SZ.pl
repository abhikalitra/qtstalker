# qtstalker SZ indicator

$barsName = 'Bars';

$szName = 'SZ';
$szStyle = 'Line';
$szColor = 'yellow';
$szPeriod = 10;
$noDeclinePeriod = 2;
$coeff = 2;

$method = 'Long';

###################################################################

$|++;

$command = "PLUGIN=OHLC,NAME=$barsName,COLOR_UP=green,COLOR_DOWN=red,COLOR_NEUTRAL=blue";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT,NAME=$barsName,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=SZ,NAME=$szName,METHOD=$method,PERIOD=$szPeriod,NO_DECLINE_PERIOD=$noDeclinePeriod,COEFFICIENT=$coeff";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$szName,STYLE=$szStyle,COLOR=$szColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
