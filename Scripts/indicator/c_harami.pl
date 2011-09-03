# qtstalker c_harami - Candles: Harami and Harami Cross

$candleName = 'candles';
$candleColor = 'dimgray';

$haramiName = 'ch1';
$haramiCrossName = 'ch2';
$bullHaramiColor = 'green';
$bullHaramiCrossColor = 'cyan';
$bearHaramiColor = 'red';
$bearHaramiCrossColor = 'magenta';

$dateName = 'Date';
$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';

$chartName = 'c_harami';

###########################################################

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

# get the harami pattern
$command = "COMMAND=CANDLE_PATTERN;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            OUTPUT=$haramiName;
            METHOD=HARAMI";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the candles
$command = "COMMAND=PLOT_OHLC;
            CHART=$chartName;
            OUTPUT=$candleName;
            STYLE=Candle;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            COLOR=$candleColor;
            Z=0;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color bear harami
$command = "COMMAND=COLOR;
            INPUT_1=$haramiName;
            INPUT_1_OFFSET=0;
            OP=LT;
            INPUT_2=0;
            INPUT_2_OFFSET=0;
            INPUT_3=$candleName;
            INPUT_3_OFFSET=0;
            COLOR=$bearHaramiColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color bull harami
$command = "COMMAND=COLOR;
            INPUT_1=$haramiName;
            INPUT_1_OFFSET=0;
            OP=GT;
            INPUT_2=0;
            INPUT_2_OFFSET=0;
            INPUT_3=$candleName;
            INPUT_3_OFFSET=0;
            COLOR=$bullHaramiColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the harami cross pattern
$command = "COMMAND=CANDLE_PATTERN;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            OUTPUT=$haramiCrossName;
            METHOD=HARAMICROSS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# color bear harami cross
$command = "COMMAND=COLOR;
            INPUT_1=$haramiCrossName;
            INPUT_1_OFFSET=0;
            OP=LT;
            INPUT_2=0;
            INPUT_2_OFFSET=0;
            INPUT_3=$candleName;
            INPUT_3_OFFSET=0;
            COLOR=$bearHaramiCrossColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color bull harami cross
$command = "COMMAND=COLOR;
            INPUT_1=$haramiCrossName;
            INPUT_1_OFFSET=0;
            OP=GT;
            INPUT_2=0;
            INPUT_2_OFFSET=0;
            INPUT_3=$candleName;
            INPUT_3_OFFSET=0;
            COLOR=$bullHaramiCrossColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update chart
$command = "COMMAND=CHART_UPDATE;
            CHART=$chartName;
            DATE=$dateName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
