# qtstalker OHLC + 3 MA indicator

# general parms
$chartName = 'OHLC_3MA';
$dateName = 'date';
$openName = 'open';
$highName = 'high';
$lowName = 'low';
$closeName = 'close';
$volumeName = 'volume';
$oiName = 'oi';

# OHLC parms
$ohlcName = 'ohlc';
$ohlcLabel = 'OHLC';
$ohlcColor = 'blue';
$ohlcZ = '0';
$ohlcUpColor = 'green';
$ohlcDownColor = 'red';

# MA1 parms
$ma1Data = 'ma1';
$ma1Label = 'MA20';
$ma1Name = 'ma20';
$ma1Color = 'yellow';
$ma1Z = '1';
$ma1Period = '20';
$ma1Type = 'EMA';
$ma1Style = 'Line';

# MA2 parms
$ma2Data = 'ma2';
$ma2Label = 'MA50';
$ma2Name = 'ma50';
$ma2Color = 'red';
$ma2Z = '1';
$ma2Period = '50';
$ma2Type = 'EMA';
$ma2Style = 'Line';

# MA3 parms
$ma3Data = 'ma3';
$ma3Label = 'MA200';
$ma3Name = 'ma200';
$ma3Color = 'blue';
$ma3Z = '1';
$ma3Period = '200';
$ma3Type = 'EMA';
$ma3Style = 'Line';

###################################################################

$|++;

# create the chart
$command = "CHART($chartName, 1, 0, 0, 0)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# load current bars
$command = "SYMBOL_CURRENT($dateName, $openName, $highName, $lowName, $closeName, $volumeName, $oiName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot OHLC bars
$command = "PLOT_OHLC($ohlcName, $chartName, $ohlcLabel, OHLC, $ohlcColor,
                      $ohlcZ, 1, $openName, $highName, $lowName, $closeName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color up bars
$command = "COMPARE($closeName, >, $closeName.1, $ohlcName, $ohlcUpColor)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color down bars
$command = "COMPARE($closeName, <, $closeName.1, $ohlcName, $ohlcDownColor)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create MA1
$command = "MA($ma1Data, $closeName, $ma1Period, $ma1Type)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create MA2
$command = "MA($ma2Data, $closeName, $ma2Period, $ma2Type)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create MA3
$command = "MA($ma3Data, $closeName, $ma3Period, $ma3Type)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot MA1
$command = "PLOT_LINE($ma1Name, $chartName, $ma1Label, $ma1Style, $ma1Color, $ma1Z, 1, $ma1Data)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot MA2
$command = "PLOT_LINE($ma2Name, $chartName, $ma2Label, $ma2Style, $ma2Color, $ma2Z, 1, $ma2Data)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot MA3
$command = "PLOT_LINE($ma3Name, $chartName, $ma3Label, $ma3Style, $ma3Color, $ma3Z, 1, $ma3Data)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update chart
$command = "CHART_UPDATE($chartName, $dateName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
