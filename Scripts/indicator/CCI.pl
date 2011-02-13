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

$command = "PLUGIN=CCI,NAME=$cciName,PERIOD=$cciPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MA,METHOD=$maType,NAME=$maName,INPUT=$cciName,PERIOD=$maPeriod";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE_TEMP,PRICE=$refUp,COLOR=$refUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE_TEMP,PRICE=$refDown,COLOR=$refDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$cciName,STYLE=$cciStyle,COLOR=$cciColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$maName,STYLE=$maStyle,COLOR=$maColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
