# qtstalker MFI indicator

$mfiName = 'MFI';
$mfiStyle = 'Line';
$mfiColor = 'red';
$mfiPeriod = 2;

$maName = 'MFI_MA';
$maStyle = 'Line';
$maColor = 'yellow';
$maPeriod = 10;
$maType = 'EMA';

$refUpColor = 'white';
$refUp = 80;

$refDownColor = 'white';
$refDown = 20;

###################################################################

$|++;

$command = "MFI,$mfiName,$mfiPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "MA,$maType,$maName,$mfiName,maPeriod";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,$refDown,$refDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,$refUp,$refUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$mfiName,$mfiStyle,$mfiColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$maName,$maStyle,$maColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
