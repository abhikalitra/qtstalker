# qtstalker AD indicator

$chartName = 'AD';
$dateName = 'date';
$openName = 'open';
$highName = 'high';
$lowName = 'low';
$closeName = 'close';
$volumeName = 'volume';
$oiName = 'oi';

$adData = 'ad';
$adName = 'AD';
$adStyle = 'Line';
$adColor = 'yellow';
$adZ = '0';

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

# create AD
$command = "COMMAND=AD;
            OUTPUT=$adData;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            VOLUME=$volumeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot AD
$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            OUTPUT=$adName;
            INPUT=$adData;
            STYLE=$adStyle;
            COLOR=$adColor;
            Z=$adZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update chart
$command = "COMMAND=CHART_UPDATE;
            CHART=$chartName;
            DATE=$dateName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
