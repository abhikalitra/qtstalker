# qtstalker s_harami - Candles: Harami and Harami Cross

# yellow  : Harami
# orange  : Harami Cross
#
# Positive values are bullish, negative values are bearish.

$haramiName = 'Harami';
$haramiData = 'harami';
$haramiStyle = 'Histogram Bar';
$haramiColor = 'yellow';
$haramiZ = '0';

$haramiCrossData = 'haramicross';
$haramiCrossName = 'Harami Cross';
$haramiCrossStyle = 'Histogram Bar';
$haramiCrossColor = 'orange';
$haramiCrossZ = '0';

$dateName = 'date';
$openName = 'open';
$highName = 'high';
$lowName = 'low';
$closeName = 'close';
$volumeName = 'volume';
$oiName = 'oi';
$chartName = 's_harami';

###################################################################

$|++;

# create the chart
$command = "COMMAND=CHART;
            NAME=$chartName;
            DATE=1;
            LOG=0;
            ROW=0;
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
            VOLUME=volume;
            OI=oi";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get harami pattern
$command = "COMMAND=CANDLE_PATTERN;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            OUTPUT=$haramiData;
            METHOD=HARAMI";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot harami
$command = "COMMAND=PLOT_HISTOGRAM;
            CHART=$chartName;
            HIGH=$haramiData;
            LOW=0;
            OUTPUT=$haramiName;
            STYLE=$haramiStyle;
            COLOR=$haramiColor;
            Z=$haramiZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get haramicross pattern
$command = "COMMAND=CANDLE_PATTERN;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            OUTPUT=$haramiCrossData;
            METHOD=HARAMICROSS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot haramicross
$command = "COMMAND=PLOT_HISTOGRAM;
            CHART=$chartName;
            HIGH=$haramiCrossData;
            LOW=0;
            OUTPUT=$haramiCrossName;
            STYLE=$haramiCrossStyle;
            COLOR=$haramiCrossColor;
            Z=$haramiCrossZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# update chart
$command = "COMMAND=CHART_UPDATE;
            CHART=$chartName;
            DATE=$dateName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
