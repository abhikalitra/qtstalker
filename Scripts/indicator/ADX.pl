# qtstalker ADX indicator

# general parms
$chartName = 'ADX';
$dateName = 'date';
$openName = 'open';
$highName = 'high';
$lowName = 'low';
$closeName = 'close';
$volumeName = 'volume';
$oiName = 'oi';
$adxPeriod = '14';

# pdi parms
$pdiData = 'pdi';
$pdiName = 'PDI';
$pdiStyle = 'Line';
$pdiColor = 'green';
$pdiZ = '0';

# mdi parms
$mdiData = 'mdi';
$mdiName = 'MDI';
$mdiStyle = 'Line';
$mdiColor = 'red';
$mdiZ = '0';

# adx parms
$adxData = 'adx';
$adxName = 'ADX';
$adxStyle = 'Line';
$adxColor = 'yellow';
$adxZ = '1';

# adxr parms
$adxrData = 'adxr';
$adxrName = 'ADXR';
$adxrStyle = 'Line';
$adxrColor = 'blue';
$adxrZ = '2';

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

# create PDI
$command = "COMMAND=ADX;
            OUTPUT=$pdiData;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            PERIOD=$adxPeriod;
            METHOD=PDI";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot PDI
$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            OUTPUT=$pdiName;
            INPUT=$pdiData;
            STYLE=$pdiStyle;
            COLOR=$pdiColor;
            Z=$pdiZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create MDI
$command = "COMMAND=ADX;
            OUTPUT=$mdiData;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            PERIOD=$adxPeriod;
            METHOD=MDI";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot MDI
$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            OUTPUT=$mdiName;
            INPUT=$mdiData;
            STYLE=$mdiStyle;
            COLOR=$mdiColor;
            Z=$mdiZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create ADX
$command = "COMMAND=ADX;
            OUTPUT=$adxData;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            PERIOD=$adxPeriod;
            METHOD=ADX";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot ADX
$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            OUTPUT=$adxName;
            INPUT=$adxData;
            STYLE=$adxStyle;
            COLOR=$adxColor;
            Z=$adxZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create ADXR
$command = "COMMAND=ADX;
            OUTPUT=$adxrData;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            PERIOD=$adxPeriod;
            METHOD=ADXR";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot ADXR
$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            OUTPUT=$adxrName;
            INPUT=$adxrData;
            STYLE=$adxrStyle;
            COLOR=$adxrColor;
            Z=$adxrZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update chart
$command = "COMMAND=CHART_UPDATE;
            CHART=$chartName;
            DATE=$dateName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
