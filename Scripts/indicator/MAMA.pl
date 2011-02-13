# qtstalker MAMA indicator

$barsName = 'Bars';

$mamaName = 'MAMA';
$mamaStyle = 'Line';
$mamaColor = 'red';

$famaName = 'FAMA';
$famaStyle = 'Line';
$famaColor = 'yellow';

$fastLimit = 0.5;
$slowLimit = 0.05;

###################################################################

$|++;

$command = "PLUGIN=OHLC,NAME=$barsName,COLOR_UP=green,COLOR_DOWN=red,COLOR_NEUTRAL=blue";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT,NAME=$barsName,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MAMA,INPUT=$barsName,NAME_MAMA=$mamaName,NAME_FAMA=$famaName,LIMIT_FAST=$fastLimit,LIMIT_SLOW=$slowLimit";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$mamaName,STYLE=$mamaStyle,COLOR=$mamaColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$famaName,STYLE=$famaStyle,COLOR=$famaColor,Z=2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
