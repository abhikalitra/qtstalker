# qtstalker Volume (Volume) indicator, red is down, blue is neutral, green is up, color based on close price.

$closeName = 'cl';

$volName = 'VOL';
$volStyle = 'Histogram Bar';

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

$command = "BARS,Volume,$volName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "BARS,Close,$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$volName,$volStyle,$neutralColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_COLOR_COMPARE_INDEX,$closeName.0,>,$closeName.1,$volName.0,$upColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_COLOR_COMPARE_INDEX,$closeName.0,<,$closeName.1,$volName.0,$downColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "MA,$maType,$maName,$volName,$maPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$maName,$maStyle,$maColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
