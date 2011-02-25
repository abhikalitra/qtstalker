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

$refColor = 'white';
$ref = 20;
$ref2Color = 'white';
$ref2 = 40;

###################################################################

$|++;

$command = "PLUGIN=MDI,NAME=$mdiName,PERIOD=$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$mdiName,STYLE=$mdiStyle,COLOR=$mdiColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=PDI,NAME=$pdiName,PERIOD=$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$pdiName,STYLE=$pdiStyle,COLOR=$pdiColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=ADX,NAME=$adxName,PERIOD=$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$adxName,STYLE=$adxStyle,COLOR=$adxColor,Z=2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE_TEMP,PRICE=$ref,COLOR=$refColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE_TEMP,PRICE=$ref2,COLOR=$ref2Color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
