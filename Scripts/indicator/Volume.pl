# qtstalker VOL indicator

# general parms
$chartName = 'Vol';
$dateName = 'date';
$openName = 'open';
$highName = 'high';
$lowName = 'low';
$closeName = 'close';
$volumeName = 'volume';
$oiName = 'oi';

#vol parms
$volName = 'vol';
$volLabel = 'VOL';
$volColor = 'blue';
$volZ = '0';
$volUpColor = 'green';
$volDownColor = 'red';
$volStyle = 'Histogram Bar';

# MA parms
$maData = 'ma';
$maName = 'ma20';
$maLabel = 'MA20';
$maColor = 'yellow';
$maZ = '1';
$maPeriod = '20';
$maType = 'EMA';
$maStyle = 'Line';

###################################################################

$|++;

# create the chart
$command = "CHART($chartName, 0, 0, 1, 0)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# load current bars
$command = "SYMBOL_CURRENT($dateName, $openName, $highName, $lowName, $closeName, $volumeName, $oiName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot the histogram
$command = "PLOT_HISTOGRAM($volName, $chartName, $volLabel, $volStyle, $volColor, $volZ, 1, $volumeName, 0)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color up bars
$command = "COMPARE($closeName, >, $closeName.1, $volName, $volUpColor)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color down bars
$command = "COMPARE($closeName, <, $closeName.1, $volName, $volDownColor)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# MA
$command = "MA($maData, $volumeName, $maPeriod, $maType)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot the MA
$command = "PLOT_LINE($maName, $chartName, $maLabel, $maStyle, $maColor, $maZ, 1, $maData)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update chart
$command = "CHART_UPDATE($chartName, $dateName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
