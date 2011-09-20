# qtstalker OHLC + 3 MA indicator

# general parms
$chartName = 'OHLC';
$dateName = 'date';
$openName = 'open';
$highName = 'high';
$lowName = 'low';
$closeName = 'close';
$volumeName = 'volume';
$oiName = 'oi';

# OHLC parms
$ohlcName = 'OHLC';
$ohlcColor = 'blue';
$ohlcZ = '0';
$ohlcUpColor = 'green';
$ohlcDownColor = 'red';

# MA1 parms
$ma1Data = 'ma1';
$ma1Name = 'MA20';
$ma1Color = 'yellow';
$ma1Z = '1';
$ma1Period = '20';
$ma1Type = 'EMA';
$ma1Style = 'Line';

# MA2 parms
$ma2Data = 'ma2';
$ma2Name = 'MA50';
$ma2Color = 'red';
$ma2Z = '1';
$ma2Period = '50';
$ma2Type = 'EMA';
$ma2Style = 'Line';

# MA3 parms
$ma3Data = 'ma3';
$ma3Name = 'MA200';
$ma3Color = 'blue';
$ma3Z = '1';
$ma3Period = '200';
$ma3Type = 'EMA';
$ma3Style = 'Line';

###################################################################

$|++;

# create the chart
$command = "COMMAND=CHART;
            NAME=$chartName;
            DATE=1;
            LOG=0;
            ROW=0;
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

# plot OHLC bars
$command = "COMMAND=PLOT_OHLC;
            CHART=$chartName;
            OUTPUT=$ohlcName;
            STYLE=OHLC;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            COLOR=$ohlcColor;
            Z=$ohlcZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color up bars
$command = "COMMAND=COLOR;
            INPUT_1=$closeName;
            OP=GT;
            INPUT_2=$closeName.1;
            INPUT_3=$ohlcName;
            COLOR=$ohlcUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color down bars
$command = "COMMAND=COLOR;
            INPUT_1=$closeName;
            OP=LT;
            INPUT_2=$closeName.1;
            INPUT_3=$ohlcName;
            COLOR=$ohlcDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create MA1
$command = "COMMAND=MA;
            OUTPUT=$ma1Data;
            INPUT=$closeName;
            PERIOD=$ma1Period;
            METHOD=$ma1Type";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create MA2
$command = "COMMAND=MA;
            OUTPUT=$ma2Data;
            INPUT=$closeName;
            PERIOD=$ma2Period;
            METHOD=$ma2Type";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create MA3
$command = "COMMAND=MA;
            OUTPUT=$ma3Data;
            INPUT=$closeName;
            PERIOD=$ma3Period;
            METHOD=$ma3Type";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot MA1
$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            OUTPUT=$ma1Name;
            INPUT=$ma1Data;
            STYLE=$ma1Style;
            COLOR=$ma1Color;
            Z=$ma1Z;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot MA2
$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            OUTPUT=$ma2Name;
            INPUT=$ma2Data;
            STYLE=$ma2Style;
            COLOR=$ma2Color;
            Z=$ma2Z;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot MA3
$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            OUTPUT=$ma3Name;
            INPUT=$ma3Data;
            STYLE=$ma3Style;
            COLOR=$ma3Color;
            Z=$ma3Z;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update chart
$command = "COMMAND=CHART_UPDATE;
            CHART=$chartName;
            DATE=$dateName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
