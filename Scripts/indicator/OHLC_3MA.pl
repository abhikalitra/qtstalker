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
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the NAME, COL values
$command = "SET(chart, NAME, $chartName,
                       ROW, 0,
                       COL, 0)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# create the chart
$command = "RUN(chart)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  LOAD QUOTES  ################################
###################################################################

# create the SYMBOL_CURRENT object
$command = "NEW(SYMBOL_CURRENT, symbol_current)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# no need to set any values, use the defaults

# load the quotes
$command = "RUN(symbol_current)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  OHLC BARS  ################################
###################################################################

# create the OHLC object
$command = "NEW(PLOT_OHLC, plot_ohlc)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set values
$command = "SET(plot_ohlc, OUTPUT, $ohlcName,
                           CHART, $chartName,
                           LABEL, $ohlcLabel,
                           Z, $ohlcZ,
                           COLOR, $ohlcColor)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# create the bars
$command = "RUN(plot_ohlc)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  COLOR OHLC BARS UP COLOR #####################
###################################################################

# create the COMPARE object
$command = "NEW(COMPARE, compare)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
$command = "SET(compare, INPUT, $closeName,
                         OP, >,
                         INPUT2, $closeName,
                         INPUT2_OFFSET, 1,
                         OUTPUT, $ohlcName,
                         OUTPUT2, $ohlcUpColor)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the compare
$command = "RUN(compare)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  COLOR OHLC BARS DOWN COLOR #####################
###################################################################

# set the values
$command = "SET(compare, OP, <, 
                         OUTPUT2, $ohlcDownColor)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the compare
$command = "RUN(compare)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  CREATE MA1 #####################
###################################################################

# create the MA object
$command = "NEW(MA, ma)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
$command = "SET(ma, INPUT, $closeName,
                    OUTPUT, $ma1Data,
                    PERIOD, $ma1Period,
                    TYPE, $ma1Type)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the MA
$command = "RUN(ma)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  CREATE MA2 #####################
###################################################################

# set the values
$command = "SET(ma, OUTPUT, $ma2Data,
                    PERIOD, $ma2Period,
                    TYPE, $ma2Type)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the MA
$command = "RUN(ma)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  CREATE MA3 #####################
###################################################################

# set the values
$command = "SET(ma, OUTPUT, $ma3Data,
                    PERIOD, $ma3Period,
                    TYPE, $ma3Type)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the MA
$command = "RUN(ma)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  PLOT MA1 #####################
###################################################################

# create the PLOT_LINE object
$command = "NEW(PLOT_LINE, plot_line)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
$command = "SET(plot_line, OUTPUT, $ma1Name,
                           CHART, $chartName,
                           LABEL, $ma1Label,
                           STYLE, $ma1Style,
                           COLOR, $ma1Color,
                           Z, $ma1Z,
                           INPUT, $ma1Data)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# add MA1 to the chart
$command = "RUN(plot_line)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  PLOT MA2 #####################
###################################################################

# set the values
$command = "SET(plot_line, OUTPUT, $ma2Name,
                           LABEL, $ma2Label,
                           STYLE, $ma2Style,
                           COLOR, $ma2Color,
                           Z, $ma2Z,
                           INPUT, $ma2Data)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# add MA2 to the chart
$command = "RUN(plot_line)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  PLOT MA3 #####################
###################################################################

# set the values
$command = "SET(plot_line, OUTPUT, $ma3Name,
                           LABEL, $ma3Label,
                           STYLE, $ma3Style,
                           COLOR, $ma3Color,
                           Z, $ma3Z,
                           INPUT, $ma3Data)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# add MA3 to the chart
$command = "RUN(plot_line)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  UPDATE CHART #####################
###################################################################

# create the CHART_UPDATE object
$command = "NEW(CHART_UPDATE, chart_update)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
$command = "SET(chart_update, CHART, $chartName,
                              DATE, $dateName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# update the chart
$command = "RUN(chart_update)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
