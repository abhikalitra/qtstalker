# qtstalker VOL indicator

# commands
$chartCommand = 'chartCommand';
$symbolCommand = 'symbolCommand';
$plotHistogramCommand = 'plotHistogramCommand';
$compareCommand = 'compareCommand';
$maCommand = 'maCommand';
$plotLineCommand = 'plotLineCommand';
$chartUpdateCommand = 'chartUpdateCommand';

# general parms
$chartName = 'Volume';
$dateName = 'date';
$closeName = 'close';
$volumeName = 'volume';

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
#  CREATE CHART
###################################################################

# create the chart object
print STDOUT "NEW(CHART, $chartCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
print STDOUT "SET($chartCommand.NAME, $chartName,
                $chartCommand.COL, 0,
                $chartCommand.DATE, 0,
                $chartCommand.ROW, 1)";
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
#  PLOT HISTOGRAM
###################################################################

# create the PLOT_LINE object
print STDOUT "NEW(PLOT_HISTOGRAM, $plotHistogramCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
print STDOUT "SET($plotHistogramCommand.OUTPUT, $volName,
                  $plotHistogramCommand.CHART, $chartName,
                  $plotHistogramCommand.LABEL, $volLabel,
                  $plotHistogramCommand.STYLE, $volStyle,
                  $plotHistogramCommand.COLOR, $volColor,
                  $plotHistogramCommand.Z, $volZ,
                  $plotHistogramCommand.HIGH, $volumeName,
                  $plotHistogramCommand.LOW, 0)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# add histogram to the chart
print STDOUT "RUN($plotHistogramCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  COLOR BARS UP COLOR
###################################################################

# create the COMPARE object
print STDOUT "NEW(COMPARE, $compareCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
print STDOUT "SET($compareCommand.INPUT, $closeName,
                  $compareCommand.OP, >,
                  $compareCommand.INPUT2, $closeName,
                  $compareCommand.INPUT2_OFFSET, 1,
                  $compareCommand.OUTPUT, $volName,
                  $compareCommand.OUTPUT2, $volUpColor)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the compare
print STDOUT "RUN($compareCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  COLOR BARS DOWN COLOR
###################################################################

# set the values
print STDOUT "SET($compareCommand.OP, <,
                  $compareCommand.OUTPUT2, $volDownColor)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the compare
print STDOUT "RUN($compareCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  CREATE MA
###################################################################

# create the MA object
print STDOUT "NEW(MA, $maCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
print STDOUT "SET($maCommand.INPUT, $volumeName,
                  $maCommand.OUTPUT, $maData,
                  $maCommand.PERIOD, $maPeriod,
                  $maCommand.TYPE, $maType)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# do the MA
print STDOUT "RUN($maCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  PLOT MA
###################################################################

# create the PLOT_LINE object
print STDOUT "NEW(PLOT_LINE, $plotLineCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the values
print STDOUT "SET($plotLineCommand.OUTPUT, $maName,
                  $plotLineCommand.CHART, $chartName,
                  $plotLineCommand.LABEL, $maLabel,
                  $plotLineCommand.STYLE, $maStyle,
                  $plotLineCommand.COLOR, $maColor,
                  $plotLineCommand.Z, $maZ,
                  $plotLineCommand.INPUT, $maData)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# add MA1 to the chart
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
