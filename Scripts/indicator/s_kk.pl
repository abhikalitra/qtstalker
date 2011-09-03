# qtstalker s_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

# yellow : Piercing Line
# orange : Dark Cloud Cover

$pData = 'piercingline';
$pName = 'Piercing Line';
$pStyle = 'Histogram Bar';
$pColor = 'yellow';
$pZ = '0';

$dccData = 'darkcloudcover';
$dccName = 'Dark Cloud Cover';
$dccStyle = 'Histogram Bar';
$dccColor = 'orange';
$dccZ = '0';

$penetration = '0.5'; # Use 50 for more reliable half-way penetration.

$dateName = 'date';
$openName = 'open';
$highName = 'high';
$lowName = 'low';
$closeName = 'close';
$volumeName = 'volume';
$oiName = 'oi';
$chartName = 's_kk';

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

# get piercing pattern
$command = "COMMAND=CANDLE_PATTERN;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            OUTPUT=$pData;
            METHOD=PIERCING";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot piercing
$command = "COMMAND=PLOT_HISTOGRAM;
            CHART=$chartName;
            HIGH=$pData;
            LOW=0;
            OUTPUT=$pName;
            STYLE=$pStyle;
            COLOR=$pColor;
            Z=$pZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get dark cloud cover pattern
$command = "COMMAND=CANDLE_PATTERN;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            OUTPUT=$dccData;
            METHOD=DARKCLOUDCOVER";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot dark cloud cover
$command = "COMMAND=PLOT_HISTOGRAM;
            CHART=$chartName;
            HIGH=$dccData;
            LOW=0;
            OUTPUT=$dccName;
            STYLE=$dccStyle;
            COLOR=$dccColor;
            Z=$dccZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# update chart
$command = "COMMAND=CHART_UPDATE;
            CHART=$chartName;
            DATE=$dateName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
