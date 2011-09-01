# qtstalker RSI indicator

$chartName = 'RSI';
$dateName = 'date';
$openName = 'open';
$highName = 'high';
$lowName = 'low';
$closeName = 'close';
$volumeName = 'volume';
$oiName = 'oi';

$rsiData = 'rsi';
$rsiName = 'RSI';
$rsiPeriod = '14';
$rsiStyle = 'Line';
$rsiColor = 'yellow';
$rsiZ = '0';

$buyZoneColor = 'green';
$buyZonePrice = '30';
$buyZoneZ = '1';

$sellZoneColor = 'red';
$sellZonePrice = '70';
$sellZoneZ = '1';

###################################################################

$|++;

# create the chart
$command = "COMMAND=CHART;
            NAME=$chartName;
            DATE=0;
            LOG=0;
            ROW=1;
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
            VOLUME=$volumeName;
            OI=$oiName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create RSI
$command = "COMMAND=RSI;
            OUTPUT=$rsiData;
            INPUT=$closeName;
            PERIOD=$rsiPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot RSI
$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            NAME=$rsiName;
            INPUT=$rsiData;
            STYLE=$rsiStyle;
            COLOR=$rsiColor;
            Z=$rsiZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create buy zone
$command = "COMMAND=CHART_OBJECT_HLINE;
            CHART=$chartName;
            PRICE=$buyZonePrice;
            COLOR=$buyZoneColor;
            Z=$buyZoneZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create sell zone
$command = "COMMAND=CHART_OBJECT_HLINE;
            CHART=$chartName;
            PRICE=$sellZonePrice;
            COLOR=$sellZoneColor;
            Z=$sellZoneZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update chart
$command = "COMMAND=CHART_UPDATE;
            CHART=$chartName;
            DATE=$dateName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
