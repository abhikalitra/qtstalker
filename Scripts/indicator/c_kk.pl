# qtstalker c_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

$candleName = 'candles';
$candleColor = 'dimgray';
$penetration = '0.5'; # Use 0.5 for more reliable half-way penetration.

$ckk1Name = 'ckk1'; # Piercing Line
$ckk2Name = 'ckk2'; # Dark Cloud Cover
$ckk1Color = 'green'; # Piercing Line Color
$ckk2Color = 'cyan'; # Dark Cloud Cover Color

$dateName = 'Dpen';
$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';

$chartName = 'c_kk';

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

# get the piercing pattern
$command = "COMMAND=CANDLE_PATTERN;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            OUTPUT=$ckk1Name;
            METHOD=PIERCING;
            PENETRATION=$penetration";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the darkcloudcover pattern
$command = "COMMAND=CANDLE_PATTERN;
            OPEN=$openName;
            HIGH=$highName;
            LOW=$lowName;
            CLOSE=$closeName;
            OUTPUT=$ckk2Name;
            METHOD=DARKCLOUDCOVER;
            PENETRATION=$penetration";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the candles
$command = "COMMAND=PLOT_OHLC;
            CHART=$chartName;
            NAME=$candleName;
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

# color piercing
$command = "COMMAND=COLOR;
            INPUT_1=$ckk1Name;
            INPUT_1_OFFSET=0;
            OP=GT;
            INPUT_2=0;
            INPUT_2_OFFSET=0;
            INPUT_3=$candleName;
            INPUT_3_OFFSET=0;
            COLOR=$ckk1Color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color darkcloudcover
$command = "COMMAND=COLOR;
            INPUT_1=$ckk2Name;
            INPUT_1_OFFSET=0;
            OP=LT;
            INPUT_2=0;
            INPUT_2_OFFSET=0;
            INPUT_3=$candleName;
            INPUT_3_OFFSET=0;
            COLOR=$ckk2Color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update chart
$command = "COMMAND=CHART_UPDATE;
            CHART=$chartName;
            DATE=$dateName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
