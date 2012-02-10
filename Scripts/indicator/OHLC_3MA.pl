# qtstalker OHLC + 3 MA indicator

# commands
$chartCommand = 'chartCommand';
$symbolCommand = 'symbolCommand';
$plotOHLCCommand = 'plotOHLCCommand';
$compareCommand = 'compareCommand';
$maCommand = 'maCommand';
$plotLineCommand = 'plotLineCommand';
$chartUpdateCommand = 'chartUpdateCommand';

# general parms
$chartName = 'OHLC_3MA';
$dateName = 'date';
$closeName = 'close';

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
$|++;  # flush buffers
###################################################################

###################################################################
#  CREATE CHART
###################################################################

# create the chart object
print STDOUT "NEW(CHART, $chartCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
print STDOUT "SET($chartCommand.NAME, $chartName,
                  $chartCommand.ROW, 0,
                  $chartCommand.COL, 0)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# create the chart
print STDOUT "RUN($chartCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  LOAD QUOTES
###################################################################

# create the SYMBOL_CURRENT object
print STDOUT "NEW(SYMBOL_CURRENT, $symbolCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# load the quotes
print STDOUT "RUN($symbolCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  OHLC BARS
###################################################################

# create the OHLC object
print STDOUT "NEW(PLOT_OHLC, $plotOHLCCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set values
print STDOUT "SET($plotOHLCCommand.OUTPUT, $ohlcName,
                  $plotOHLCCommand.CHART, $chartName,
                  $plotOHLCCommand.LABEL, $ohlcLabel,
                  $plotOHLCCommand.Z, $ohlcZ,
                  $plotOHLCCommand.COLOR, $ohlcColor)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# create the bars
print STDOUT "RUN($plotOHLCCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  COLOR OHLC BARS UP COLOR
###################################################################

# create the COMPARE object
print STDOUT "NEW(COMPARE, $compareCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
print STDOUT "SET($compareCommand.INPUT, $closeName,
                  $compareCommand.OP, >,
                  $compareCommand.INPUT2, $closeName,
                  $compareCommand.INPUT2_OFFSET, 1,
                  $compareCommand.OUTPUT, $ohlcName,
                  $compareCommand.OUTPUT2, $ohlcUpColor)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the compare
print STDOUT "RUN($compareCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  COLOR OHLC BARS DOWN COLOR
###################################################################

# set the values
print STDOUT "SET($compareCommand.OP, <, 
                  $compareCommand.OUTPUT2, $ohlcDownColor)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the compare
print STDOUT "RUN($compareCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  CREATE MA1
###################################################################

# create the MA object
print STDOUT "NEW(MA, $maCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
print STDOUT "SET($maCommand.INPUT, $closeName,
                  $maCommand.OUTPUT, $ma1Data,
                  $maCommand.PERIOD, $ma1Period,
                  $maCommand.TYPE, $ma1Type)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the MA
print STDOUT "RUN($maCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  CREATE MA2
###################################################################

# set the values
print STDOUT "SET($maCommand.OUTPUT, $ma2Data,
                  $maCommand.PERIOD, $ma2Period,
                  $maCommand.TYPE, $ma2Type)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the MA
print STDOUT "RUN($maCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  CREATE MA3
###################################################################

# set the values
print STDOUT "SET($maCommand.OUTPUT, $ma3Data,
                  $maCommand.PERIOD, $ma3Period,
                  $maCommand.TYPE, $ma3Type)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the MA
print STDOUT "RUN($maCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
# PLOT MA1
###################################################################

# create the PLOT_LINE object
print STDOUT "NEW(PLOT_LINE, $plotLineCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
print STDOUT "SET($plotLineCommand.OUTPUT, $ma1Name,
                  $plotLineCommand.CHART, $chartName,
                  $plotLineCommand.LABEL, $ma1Label,
                  $plotLineCommand.STYLE, $ma1Style,
                  $plotLineCommand.COLOR, $ma1Color,
                  $plotLineCommand.Z, $ma1Z,
                  $plotLineCommand.INPUT, $ma1Data)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# add MA1 to the chart
print STDOUT "RUN($plotLineCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  PLOT MA2
###################################################################

# set the values
print STDOUT "SET($plotLineCommand.OUTPUT, $ma2Name,
                  $plotLineCommand.LABEL, $ma2Label,
                  $plotLineCommand.STYLE, $ma2Style,
                  $plotLineCommand.COLOR, $ma2Color,
                  $plotLineCommand.Z, $ma2Z,
                  $plotLineCommand.INPUT, $ma2Data)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# add MA2 to the chart
print STDOUT "RUN($plotLineCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  PLOT MA3
###################################################################

# set the values
print STDOUT "SET($plotLineCommand.OUTPUT, $ma3Name,
                  $plotLineCommand.LABEL, $ma3Label,
                  $plotLineCommand.STYLE, $ma3Style,
                  $plotLineCommand.COLOR, $ma3Color,
                  $plotLineCommand.Z, $ma3Z,
                  $plotLineCommand.INPUT, $ma3Data)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# add MA3 to the chart
print STDOUT "RUN($plotLineCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  UPDATE CHART
###################################################################

# create the CHART_UPDATE object
print STDOUT "NEW(CHART_UPDATE, $chartUpdateCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
print STDOUT "SET($chartUpdateCommand.CHART, $chartName,
                  $chartUpdateCommand.DATE, $dateName)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# update the chart
print STDOUT "RUN($chartUpdateCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
