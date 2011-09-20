# qtstalker VOL indicator

# general parms
$chartName = 'VOL';
$dateName = 'date';
$openName = 'open';
$highName = 'high';
$lowName = 'low';
$closeName = 'close';
$volumeName = 'volume';
$oiName = 'oi';

#vol parms
$volName = 'VOL';
$volColor = 'blue';
$volZ = '0';
$volUpColor = 'green';
$volDownColor = 'red';
$volStyle = 'Histogram Bar';

# MA parms
$maData = 'ma';
$maName = 'MA20';
$maColor = 'yellow';
$maZ = '1';
$maPeriod = '20';
$maType = 'EMA';
$maStyle = 'Line';

###################################################################

$|++;

# create the chart
$command = "COMMAND=CHART;
            NAME=$chartName;
            DATE=0;
            LOG=0;
            ROW=1;
            COL=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

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
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot the histogram
$command = "COMMAND=PLOT_HISTOGRAM;
            CHART=$chartName;
            HIGH=$volumeName;
            LOW=0;
            OUTPUT=$volName;
            STYLE=$volStyle;
            COLOR=$volColor;
            Z=$volZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color up bars
$command = "COMMAND=COLOR;
            INPUT_1=$closeName;
            OP=GT;
            INPUT_2=$closeName.1;
            INPUT_3=$volName;
            COLOR=$volUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color down bars
$command = "COMMAND=COLOR;
            INPUT_1=$closeName;
            OP=LT;
            INPUT_2=$closeName.1;
            INPUT_3=$volName;
            COLOR=$volDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# MA
$command = "COMMAND=MA;
            OUTPUT=$maData;
            INPUT=$volumeName;
            PERIOD=$maPeriod;
            METHOD=$maType";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot the MA
$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            OUTPUT=$maName;
            INPUT=$maData;
            STYLE=$maStyle;
            COLOR=$maColor;
            Z=$maZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update chart
$command = "COMMAND=CHART_UPDATE;
            CHART=$chartName;
            DATE=$dateName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
