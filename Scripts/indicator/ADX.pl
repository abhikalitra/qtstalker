# qtstalker ADX indicator

$mdiName = '-DI';
$mdiStyle = 'Line';
$mdiColor = 'red';

$pdiName = '+DI';
$pdiStyle = 'Line';
$pdiColor = 'green';

$adxName = 'ADX';
$adxStyle = 'Line';
$adxColor = 'yellow';

$period = 14;

###################################################################

$|++;

$command = "MDI,$mdiName,$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$mdiName,$mdiStyle,$mdiColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PDI,$pdiName,$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$pdiName,$pdiStyle,$pdiColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "ADX,$adxName,$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$adxName,$adxStyle,$adxColor,2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
