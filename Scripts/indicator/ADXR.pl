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

$command = "PLUGIN=ADX,NAME=$adxName,PERIOD=$adxPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$adxName,STYLE=$adxStyle,COLOR=$adxColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=ADXR,NAME=$adxrName,PERIOD=$adxrPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$adxrName,STYLE=$adxrStyle,COLOR=$adxrColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
