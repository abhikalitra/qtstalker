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
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# set the NAME, COL values
$command = "SET(chart, NAME, $chartName,
                chart, COL, 0,
                chart, DATE, 0,
                chart, ROW, 1)";
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
###################  PLOT HISTOGRAM #####################
###################################################################

# create the PLOT_LINE object
$command = "NEW(PLOT_HISTOGRAM, plot_histogram)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# set the values
$command = "SET(plot_histogram, OUTPUT, $volName,
                plot_histogram, CHART, $chartName,
                plot_histogram, LABEL, $volLabel,
                plot_histogram, STYLE, $volStyle,
                plot_histogram, COLOR, $volColor,
                plot_histogram, Z, $volZ,
                plot_histogram, HIGH, $volumeName,
                plot_histogram, LOW, 0)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add histogram to the chart
$command = "RUN(plot_histogram)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  COLOR BARS UP COLOR #####################
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
                compare, OUTPUT, $volName,
                compare, OUTPUT2, $volUpColor)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# do the compare
$command = "RUN(compare)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  COLOR BARS DOWN COLOR #####################
###################################################################

# set the values
$command = "SET(compare, OP, <,
                compare, OUTPUT2, $volDownColor)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# do the compare
$command = "RUN(compare)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  CREATE MA #####################
###################################################################

# create the MA object
$command = "NEW(MA, ma)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# set the values
$command = "SET(ma, INPUT, $volumeName,
                ma, OUTPUT, $maData,
                ma, PERIOD, $maPeriod,
                ma, TYPE, $maType)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# do the MA
$command = "RUN(ma)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

###################################################################
###################  PLOT MA #####################
###################################################################

# create the PLOT_LINE object
$command = "NEW(PLOT_LINE, plot_line)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# set the values
$command = "SET(plot_line, OUTPUT, $maName,
                plot_line, CHART, $chartName,
                plot_line, LABEL, $maLabel,
                plot_line, STYLE, $maStyle,
                plot_line, COLOR, $maColor,
                plot_line, Z, $maZ,
                plot_line, INPUT, $maData)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add MA1 to the chart
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
