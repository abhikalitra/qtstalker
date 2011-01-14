# qtstalker AROON indicator

$aroonUpName = 'AROON+';
$aroonUpStyle = 'Line';
$aroonUpColor = 'green';

$aroonDownName = 'AROON-';
$aroonDownStyle = 'Line';
$aroonDownColor = 'red';

$period = 14;

###################################################################

$|++;

$command = "AROON,$aroonUpName,$aroonDownName,$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$aroonUpName,$aroonUpStyle,$aroonUpColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$aroonDownName,$aroonDownStyle,$aroonDownColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
