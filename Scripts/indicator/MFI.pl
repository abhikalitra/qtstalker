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

$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';
$volName = 'Volume';

###################################################################

$|++;

$command = "PLUGIN=HIGH,NAME=$highName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=LOW,NAME=$lowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CLOSE,NAME=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=VOLUME,NAME=$volName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MFI,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,INPUT_VOLUME=$volName,NAME=$mfiName,PERIOD=$mfiPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MA,METHOD=$maType,NAME=$maName,INPUT=$mfiName,PERIOD=$maPeriod";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE,TYPE=RO,PRICE=$refDown,COLOR=$refDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE,TYPE=RO,PRICE=$refUp,COLOR=$refUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$mfiName,STYLE=$mfiStyle,COLOR=$mfiColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$maName,STYLE=$maStyle,COLOR=$maColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
