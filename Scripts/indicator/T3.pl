# qtstalker T3 indicator

$barsName = 'Bars';

$t3Name = 'T3';
$t3Style = 'Line';
$t3Color = 'yellow';
$t3Period = 5;
$vfactor = 0.7;

$method = 'Long';

###################################################################

$|++;

$command = "BARS,BARS,$barsName,green,red,blue";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,$barsName,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "T3,$t3Name,$barsName,$t3Period,$vfactor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$t3Name,$t3Style,$t3Color,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
