# qtstalker SAR indicator

$barsName = 'Bars';

$sarName = 'SAR';
$sarStyle = 'Dot';
$sarColor = 'white';
$initial = 0.02;
$maxStep = 0.2;

###################################################################

$|++;

$command = "BARS,BARS,$barsName,green,red,blue";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,$barsName,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "SAR,$sarName,$initial,$maxStep";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$sarName,$sarStyle,$sarColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
