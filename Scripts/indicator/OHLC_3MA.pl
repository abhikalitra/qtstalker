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
$|++;  # flush buffers
###################################################################

###################################################################
###################  CREATE CHART  ################################
###################################################################

# create the chart object
$command = "NEW(CHART, chart)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# set the NAME, COL values
$command = "SET(chart, NAME, $chartName,
                chart, COL, 0)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create the chart
$command = "RUN(chart)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  LOAD QUOTES  ################################
###################################################################

# create the SYMBOL_CURRENT object
$command = "NEW(SYMBOL_CURRENT, symbol_current)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# no need to set any values, use the defaults

# load the quotes
$command = "RUN(symbol_current)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  OHLC BARS  ################################
###################################################################

# create the OHLC object
$command = "NEW(PLOT_OHLC, plot_ohlc)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# set values
$command = "SET(plot_ohlc, OUTPUT, $ohlcName,
                plot_ohlc, CHART, $chartName,
                plot_ohlc, LABEL, $ohlcLabel,
                plot_ohlc, COLOR, $ohlcColor)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create the bars
$command = "RUN(plot_ohlc)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  COLOR OHLC BARS UP COLOR #####################
###################################################################

# create the COMPARE object
$command = "NEW(COMPARE, compare)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# set the values
$command = "SET(compare, INPUT, $closeName,
                compare, OP, >,
                compare, INPUT2, $closeName,
                compare, INPUT2_OFFSET, 1,
                compare, OUTPUT, $ohlcName,
                compare, OUTPUT2, $ohlcUpColor)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# do the compare
$command = "RUN(compare)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  COLOR OHLC BARS DOWN COLOR #####################
###################################################################

# set the values
$command = "SET(compare, OP, <,
                compare, OUTPUT2, $ohlcDownColor)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# do the compare
$command = "RUN(compare)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  CREATE MA1 #####################
###################################################################

# create the MA object
$command = "NEW(MA, ma)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# set the values
$command = "SET(ma, INPUT, $closeName,
                ma, OUTPUT, $ma1Data,
                ma, PERIOD, $ma1Period,
                ma, TYPE, $ma1Type)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# do the MA
$command = "RUN(ma)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  CREATE MA2 #####################
###################################################################

# set the values
$command = "SET(ma, OUTPUT, $ma2Data,
                ma, PERIOD, $ma2Period,
                ma, TYPE, $ma2Type)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# do the MA
$command = "RUN(ma)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  CREATE MA3 #####################
###################################################################

# set the values
$command = "SET(ma, OUTPUT, $ma3Data,
                ma, PERIOD, $ma3Period,
                ma, TYPE, $ma3Type)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# do the MA
$command = "RUN(ma)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  PLOT MA1 #####################
###################################################################

# create the PLOT_LINE object
$command = "NEW(PLOT_LINE, plot_line)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# set the values
$command = "SET(plot_line, OUTPUT, $ma1Name,
                plot_line, CHART, $chartName,
                plot_line, LABEL, $ma1Label,
                plot_line, STYLE, $ma1Style,
                plot_line, COLOR, $ma1Color,
                plot_line, Z, $ma1Z,
                plot_line, INPUT, $ma1Data)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add MA1 to the chart
$command = "RUN(plot_line)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  PLOT MA2 #####################
###################################################################

# set the values
$command = "SET(plot_line, OUTPUT, $ma2Name,
                plot_line, LABEL, $ma2Label,
                plot_line, STYLE, $ma2Style,
                plot_line, COLOR, $ma2Color,
                plot_line, Z, $ma2Z,
                plot_line, INPUT, $ma2Data)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add MA2 to the chart
$command = "RUN(plot_line)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  PLOT MA3 #####################
###################################################################

# set the values
$command = "SET(plot_line, OUTPUT, $ma3Name,
                plot_line, LABEL, $ma3Label,
                plot_line, STYLE, $ma3Style,
                plot_line, COLOR, $ma3Color,
                plot_line, Z, $ma3Z,
                plot_line, INPUT, $ma3Data)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add MA3 to the chart
$command = "RUN(plot_line)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  UPDATE CHART #####################
###################################################################

# create the CHART_UPDATE object
$command = "NEW(CHART_UPDATE, chart_update)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# set the values
$command = "SET(chart_update, CHART, $chartName,
                chart_update, DATE, $dateName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update the chart
$command = "RUN(chart_update)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
