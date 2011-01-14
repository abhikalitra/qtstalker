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

$command = "BARS,BARS,$barsName,green,red,blue";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,$barsName,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "SZ,$szName,$method,$szPeriod,$noDeclinePeriod,$coeff";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$szName,$szStyle,$szColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
