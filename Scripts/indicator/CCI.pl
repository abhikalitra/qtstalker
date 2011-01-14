# qtstalker CCI indicator

$cciName = 'CCI';
$cciColor = 'red';
$cciStyle = 'Line';
$cciPeriod = 20;

$maName = 'CCI_MA';
$maPeriod = 10;
$maColor = 'yellow';
$maStyle = 'Line';
$maType = 'EMA';

$refUpColor = 'white';
$refUp = 100;

$refDownColor = 'white';
$refDown = -100;

###################################################################

$|++;

$command = "CCI,$cciName,$cciPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "MA,$maType,$maName,$cciName,$maPeriod";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,$refUp,$refUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,$refDown,$refDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$cciName,$cciStyle,$cciColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$maName,$maStyle,$maColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
