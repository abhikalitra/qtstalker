## @file
# Qtstalker Disparity Index indicator
#
# @par References
# - http://tadoc.org/indicator/DISPARITY.htm
#
# @par Formula
# dix = ((100 * (close - ma)) / close)
#
# @par Legend
# - green = positive disparity
# - red = negative disparity
#

# disparity parms
$dname = 'Disparity';
$dateName = 'date';
$openName = 'open';
$highName = 'high';
$lowName = 'low';
$closeName = 'close';
$volumeName = 'volume';
$oiName = 'oi';
$chartName = 'Disparity';
$upColor = 'green';
$downColor = 'red';
$dstyle = 'Histogram Bar';
$dz = '0';

# MA parms
$maName = 'ma';
$maPeriod = '13';
$maType = 'EMA';

# diff parms
$diffName = 'diff';

# mult parms
$multName = 'mult';

# div parms
$divName = 'div';

########################################################################

$|++;

# create the chart
$command = "COMMAND=CHART;
            NAME=$chartName;
            DATE=0;
            LOG=0;
            ROW=1;
            COL=99";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# load current bars
$command = "COMMAND=SYMBOL_CURRENT;
            DATE=$dateName;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            VOLUME=$volumeName;
            OI=$oiName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the 13 period MA
$command = "COMMAND=MA;
            OUTPUT=$maName;
            INPUT=$closeName;
            PERIOD=$maPeriod;
            METHOD=$maType";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# close - 13 period MA
$command = "COMMAND=ARITHMETIC;
            OUTPUT=$diffName;
            INPUT_1=$closeName;
            INPUT_2=$maName;
            METHOD=SUB";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# (close - 13 period MA) * 100
$command = "COMMAND=ARITHMETIC;
            OUTPUT=$multName;
            INPUT_1=$diffName;
            INPUT_2=100;
            METHOD=MULT";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# mult / close
$command = "COMMAND=ARITHMETIC;
            OUTPUT=$divName;
            INPUT_1=$multName;
            INPUT_2=$closeName;
            METHOD=DIV";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot disparity as a histogram bar
$command = "COMMAND=PLOT_HISTOGRAM;
            CHART=$chartName;
            HIGH=$divName;
            LOW=0;
            NAME=$dName;
            STYLE=$dStyle;
            COLOR=$downColor;
            Z=$dZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color up bars
$command = "COMMAND=COLOR;
            INPUT_1=$divName;
            INPUT_1_OFFSET=0;
            OP=GT;
            INPUT_2=0;
            INPUT_2_OFFSET=0;
            INPUT_3=$dName;
            INPUT_3_OFFSET=0;
            COLOR=$upColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update chart
$command = "COMMAND=CHART_UPDATE;
            CHART=$chartName;
            DATE=$dateName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
