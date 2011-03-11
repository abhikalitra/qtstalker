# qtstalker ADXR indicator

$adxName = 'ADX';
$adxStyle = 'Line';
$adxColor = 'red';

$adxrName = 'ADXR';
$adxrStyle = 'Line';
$adxrColor = 'yellow';

$adxPeriod = 14;
$adxrPeriod = 10;

$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';

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

$command = "PLUGIN=ADX,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,NAME=$adxName,PERIOD=$adxPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$adxName,STYLE=$adxStyle,COLOR=$adxColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=ADXR,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,NAME=$adxrName,PERIOD=$adxrPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$adxrName,STYLE=$adxrStyle,COLOR=$adxrColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
