# qtstalker Ichimoku indicator

$candleName = 'Candles';
$candleUpColor = 'green';
$candleDownColor = 'green';
$candleNeutralColor = 'green';
$candleZ = '1';

$tsName = 'TENKAN SEN';
$tsStyle = 'Line';
$tsColor = 'red';
$tsPeriod = '9';
$tsHighName = 'tsHigh';
$tsLowName = 'tsLow';
$tsZ = '2';
$tsMedianName = 'tsmedian';

$ksName = 'KIJUN SEN';
$ksStyle = 'Line';
$ksColor = 'blue';
$ksPeriod = '26';
$ksHighName = 'ksHigh';
$ksLowName = 'ksLow';
$ksZ = '3';
$ksMedianName = 'ksmedian';

$csName = 'CHIKOU SPAN';
$csStyle = 'Line';
$csColor = 'purple';
$csPeriod = '-26';
$csZ = '4';
$csShiftName = 'csshift';

$ssaName = 'SENKOU SPAN A';
#$ssaStyle = 'Line';
#$ssaColor = 'orange';
$ssaPeriod = '26';
#$ssaZ = '4';
$ssaMedianName = 'ssamedian';
$ssaShiftName = 'ssashift';

$ssbName = 'SENKOU SPAN B';
#$ssbStyle = 'Line';
#$ssbColor = 'yellow';
$ssbPeriod = '52';
$ssbShift = '26';
#$ssbZ = '5';
$ssbHighName = 'ssbHigh';
$ssbLowName = 'ssbLow';
$ssbMedianName = 'ssbmedian';
$ssbShiftName = 'ssbshift';

$ssCloudZ = '0';
$ssCloudColor = 'dimgray';
$ssCloudName = 'SSAB';

$dateName = 'Date';
$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';

$chartName = 'Ichimoku';

###################################################################

$|++;

# candles

# create the chart
$command = "COMMAND=CHART;
            NAME=$chartName;
            DATE=1;
            LOG=0;
            ROW=0;
            COL=99";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# load current bars
$command = "COMMAND=SYMBOL_CURRENT;
            DATE=$dateName;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            VOLUME=volume;
            OI=oi";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create the candles
$command = "COMMAND=PLOT_OHLC;
            CHART=$chartName;
            NAME=$candleName;
            STYLE=Candle;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            COLOR=$candleNeutralColor;
            Z=$candleZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color up candles
$command = "COMMAND=COLOR;
            INPUT_1=$closeName;
            INPUT_1_OFFSET=0;
            OP=GT;
            INPUT_2=$closeName;
            INPUT_2_OFFSET=1;
            INPUT_3=$candleName;
            INPUT_3_OFFSET=0;
            COLOR=$candleUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color down candles
$command = "COMMAND=COLOR;
            INPUT_1=$closeName;
            INPUT_1_OFFSET=0;
            OP=LT;
            INPUT_2=$closeName;
            INPUT_2_OFFSET=1;
            INPUT_3=$candleName;
            INPUT_3_OFFSET=0;
            COLOR=$candleDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }


# TENKAN SEN

$command = "COMMAND=MINMAX;
            OUTPUT=$tsHighName;
            INPUT=$highName;
            METHOD=MAX;
            PERIOD=$tsPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=MINMAX;
            OUTPUT=$tsLowName;
            INPUT=$lowName;
            METHOD=MIN;
            PERIOD=$tsPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=MEDIAN_PRICE;
            OUTPUT=$tsMedianName;
            INPUT_1=$tsHighName;
            INPUT_2=$tsLowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            NAME=$tsName;
            INPUT=$tsMedianName;
            STYLE=$tsStyle;
            COLOR=$tsColor;
            Z=$tsZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# KIJUN SEN

$command = "COMMAND=MINMAX;
            OUTPUT=$ksHighName;
            INPUT=$highName;
            METHOD=MAX;
            PERIOD=$ksPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=MINMAX;
            METHOD=MIN;
            OUTPUT=$ksLowName;
            INPUT=$lowName;
            PERIOD=$ksPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=MEDIAN_PRICE;
            OUTPUT=$ksMedianName;
            INPUT_1=$ksHighName;
            INPUT_2=$ksLowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            NAME=$ksName;
            INPUT=$ksMedianName;
            STYLE=$ksStyle;
            COLOR=$ksColor;
            Z=$ksZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# CHIKOU SPAN

$command = "COMMAND=SHIFT;
            OUTPUT=$csShiftName;
            INPUT=$closeName;
            PERIOD=$csPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            NAME=$csName;
            INPUT=$csShiftName;
            STYLE=$csStyle;
            COLOR=$csColor;
            Z=$csZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# SENKOU SPAN A

$command = "COMMAND=MEDIAN_PRICE;
            OUTPUT=$ssaMedianName;
            INPUT_1=$tsName;
            INPUT_2=$ksName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=SHIFT;
            OUTPUT=$ssaShiftName;
            INPUT=$ssaMedianName;
            PERIOD=$ssaPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# SENKOU SPAN B

$command = "COMMAND=MINMAX;
            METHOD=MAX;
            OUTPUT=$ssbHighName;
            INPUT=$highName;
            PERIOD=$ssbPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=MINMAX;
            METHOD=MIN;
            OUTPUT=$ssbLowName;
            INPUT=$lowName;
            PERIOD=$ssbPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=MEDIAN_PRICE;
            OUTPUT=$ssbMedianName;
            INPUT_1=$ssbHighName;
            INPUT_2=$ssbLowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=SHIFT;
            OUTPUT=$ssbShiftName;
            INPUT=$ssbMedianName;
            PERIOD=$ssbPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the senkou span a -b cloud
$command = "COMMAND=PLOT_HISTOGRAM;
            CHART=$chartName;
            HIGH=$ssaShiftName;
            LOW=$ssbShiftName;
            NAME=$ssCloudName;
            STYLE=Histogram;
            COLOR=$ssCloudColor;
            Z=$ssCloudZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# update chart
$command = "COMMAND=CHART_UPDATE;
            CHART=$chartName;
            DATE=$dateName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
