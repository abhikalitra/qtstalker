# qtstalker HT PHASOR indicator

$input = 'c';

$phaseName = 'PHASE';
$phaseStyle = 'Line';
$phaseColor = 'red';

$quadName = 'QUAD';
$quadStyle = 'Line';
$quadColor = 'yellow';

###################################################################

$|++;

$command = "BARS,Close,$input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "HT_PHASOR,$input,$phaseName,$quadName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$phaseName,$phaseStyle,$phaseColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$quadName,$quadStyle,$quadColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
