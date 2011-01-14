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

$command = "BARS,BARS,$barsName,green,red,blue";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,$barsName,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "MAMA,$barsName,$mamaName,$famaName,$fastLimit,$slowLimit";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$mamaName,$mamaStyle,$mamaColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$famaName,$famaStyle,$famaColor,2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
