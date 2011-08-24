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
########################################################################

$|++;

# create the chart
$command = "COMMAND=CHART;
            STEP=createChart;
            NAME=Disparity;
            DATE=false;
            LOG=false;
            ROW=1;
            COL=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# load current bars
$command = "COMMAND=SYMBOL_CURRENT;
            STEP=loadBars;
            DATE=date;
            OPEN=open;
            HIGH=high;
            LOW=low;
            CLOSE=close;
            VOLUME=volume;
            OI=oi";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the 13 period MA
$command = "COMMAND=MA;
            STEP=createMA13;
            NAME=ma13;
            INPUT=loadBars:CLOSE;
            PERIOD=13;
            METHOD=EMA";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# close - 13 period MA
$command = "COMMAND=ARITHMETIC;
            STEP=close-MA13;
            NAME=diff;
            INPUT=loadBars:CLOSE;
            INPUT_2=createMA13:NAME;
            METHOD=SUB";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# (close - 13 period MA) * 100
$command = "COMMAND=ARITHMETIC;
            STEP=100*diff;
            NAME=mult;
            INPUT=close-MA13:NAME;
            INPUT_2=100;
            METHOD=MULT";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# mult / close
$command = "COMMAND=ARITHMETIC;
            STEP=mult/close;
            NAME=disparity;
            INPUT=100*diff:NAME;
            INPUT_2=loadBars:CLOSE;
            METHOD=DIV";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot disparity as a histogram bar
$command = "COMMAND=PLOT_HISTOGRAM_BAR;
            STEP=plotDisparity;
            CHART=createChart:NAME;
            DATE=loadBars:DATE;
            INPUT=mult/close:NAME;
            NAME=Disparity;
            COLOR=#ff0000;
            Z=0;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# color positive disparity
$command = "STEP=colorPositive;
            COMMAND=COLOR;
            NAME=plotDisparity:NAME;
            NAME_OFFSET=0;
            NAME_2=0;
            NAME_2_OFFSET=0;
            OP=GT;
            NAME_3=plotDisparity:NAME;
            NAME_3_OFFSET=0;
            COLOR=#00FF00";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update the chart
$command = "COMMAND=CHART_UPDATE;
            STEP=updateChart;
            CHART=createChart:NAME";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
