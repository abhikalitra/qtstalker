# qtstalker AROON indicator

$aroonUpName = 'AROON+';
$aroonUpStyle = 'Line';
$aroonUpColor = 'green';

$aroonDownName = 'AROON-';
$aroonDownStyle = 'Line';
$aroonDownColor = 'red';

$period = 14;
$highName = 'High';
$lowName = 'Low';

###################################################################

$|++;

$command = "PLUGIN=HIGH,NAME=$highName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=LOW,NAME=$lowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=AROON,INPUT_HIGH=$highName,INPUT_LOW=$lowName,NAME_UPPER=$aroonUpName,NAME_LOWER=$aroonDownName,PERIOD=$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$aroonUpName,STYLE=$aroonUpStyle,COLOR=$aroonUpColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$aroonDownName,STYLE=$aroonDownStyle,COLOR=$aroonDownColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
