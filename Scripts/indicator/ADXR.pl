# qtstalker ADXR indicator

$adxName = 'ADX';
$adxStyle = 'Line';
$adxColor = 'red';

$adxrName = 'ADXR';
$adxrStyle = 'Line';
$adxrColor = 'yellow';

$adxPeriod = 14;
$adxrPeriod = 10;

###################################################################

$|++;

$command = "ADX,$adxName,$adxPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$adxName,$adxStyle,$adxColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "ADXR,$adxrName,$adxrPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$adxrName,$adxrStyle,$adxrColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
