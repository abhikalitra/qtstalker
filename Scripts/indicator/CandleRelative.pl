# qtstalker CandleRelative indicator
# Coloured relative to previous Close.

$name = 'Candles';
$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';
$chartName = 'CR';
$neutralColor = 'blue';
$upColor = 'green';
$downColor = 'red';

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
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# load current bars
$command = "COMMAND=SYMBOL_CURRENT;
            DATE=$dateName;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create the candles
$command = "COMMAND=PLOT_OHLC;
            CHART=$chartName;
            NAME=$name;
            STYLE=Candle;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            COLOR=$neutralColor;
            Z=0;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color up bars
$command = "COMMAND=COLOR;
            INPUT_1=$closeName;
            INPUT_1_OFFSET=0;
            OP=GT;
            INPUT_2=$closeName;
            INPUT_2_OFFSET=1;
            INPUT_3=$name;
            INPUT_3_OFFSET=0;
            COLOR=$upColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color down bars
$command = "COMMAND=COLOR;
            INPUT_1=$closeName;
            INPUT_1_OFFSET=0;
            OP=LT;
            INPUT_2=$closeName;
            INPUT_2_OFFSET=1;
            INPUT_3=$name;
            INPUT_3_OFFSET=0;
            COLOR=$downColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update chart
$command = "COMMAND=CHART_UPDATE;
            CHART=$chartName;
            DATE=$dateName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
