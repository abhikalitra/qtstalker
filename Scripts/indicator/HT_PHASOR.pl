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

$command = "PLUGIN=OHLC,NAME=$input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=HT_PHASOR,INPUT=$input,NAME_PHASE=$phaseName,NAME_QUAD=$quadName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$phaseName,STYLE=$phaseStyle,COLOR=$phaseColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$quadName,STYLE=$quadStyle,COLOR=$quadColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
