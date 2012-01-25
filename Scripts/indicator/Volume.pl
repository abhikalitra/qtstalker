# qtstalker VOL indicator

# general parms
$chartName = 'Volume';
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
$|++; # flush buffers
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
                       COL, 0,
                       DATE, 0,
                       ROW, 1)";
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
###################  PLOT HISTOGRAM #####################
###################################################################

# create the PLOT_LINE object
$command = "NEW(PLOT_HISTOGRAM, plot_histogram)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
$command = "SET(plot_histogram, OUTPUT, $volName,
                                CHART, $chartName,
                                LABEL, $volLabel,
                                STYLE, $volStyle,
                                COLOR, $volColor,
                                Z, $volZ,
                                HIGH, $volumeName,
                                LOW, 0)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# add histogram to the chart
$command = "RUN(plot_histogram)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  COLOR BARS UP COLOR #####################
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
                         OUTPUT, $volName,
                         OUTPUT2, $volUpColor)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the compare
$command = "RUN(compare)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  COLOR BARS DOWN COLOR #####################
###################################################################

# set the values
$command = "SET(compare, OP, <,
                         OUTPUT2, $volDownColor)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the compare
$command = "RUN(compare)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  CREATE MA #####################
###################################################################

# create the MA object
$command = "NEW(MA, ma)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
$command = "SET(ma, INPUT, $volumeName,
                    OUTPUT, $maData,
                    PERIOD, $maPeriod,
                    TYPE, $maType)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the MA
$command = "RUN(ma)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  PLOT MA #####################
###################################################################

# create the PLOT_LINE object
$command = "NEW(PLOT_LINE, plot_line)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
$command = "SET(plot_line, OUTPUT, $maName,
                           CHART, $chartName,
                           LABEL, $maLabel,
                           STYLE, $maStyle,
                           COLOR, $maColor,
                           Z, $maZ,
                           INPUT, $maData)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# add MA1 to the chart
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
