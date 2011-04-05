# qtstalker Volume (Volume) indicator, red is down, blue is neutral, green is up
# color based on close relative to previous close

$closeName = 'cl';

$volName = 'VOL';
$volStyle = 'HistogramBar';

$maName = 'VOL_MA';
$maStyle = 'Line';
$maColor = 'yellow';
$maType = 'EMA';
$maPeriod = '10';

$upColor = 'green';
$downColor = 'red';
$neutralColor = 'blue';

###################################################################

$|++;

$command = "PLUGIN=DOHLCVI,METHOD=V,NAME=$volName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=DOHLCVI,METHOD=C,NAME=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$volName,STYLE=$volStyle,COLOR=$neutralColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_INDEX,NAME=$closeName.0,OP=GT,NAME2=$closeName.1,NAME3=$volName.0,COLOR=$upColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_INDEX,NAME=$closeName.0,OP=LT,NAME2=$closeName.1,NAME3=$volName.0,COLOR=$downColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MA,METHOD=$maType,NAME=$maName,INPUT=$volName,PERIOD=$maPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$maName,STYLE=$maStyle,COLOR=$maColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
