# qtstalker Bollinger Bandwidth indicator

# general parms
$chartName = 'BBW';
$dateName = 'date';
$openName = 'open';
$highName = 'high';
$lowName = 'low';
$closeName = 'close';
$volumeName = 'volume';
$oiName = 'oi';

$bbUpName = 'BB+';
$bbMidName = 'BB';
$bbDownName = 'BB-';
$period = 20;
$maType = 'SMA';
$udev = 2;
$ddev = 2;

$subName = 'sub';
$divName = 'div';

$bbwName = 'BBW';
$bbwColor = 'yellow';
$bbwStyle = 'Line';
$bbwZ = '0';

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

$command = "COMMAND=BBANDS;
            INPUT=$closeName;
            OUTPUT_UPPER=$bbUpName;
            OUTPUT_MIDDLE=$bbMidName;
            OUTPUT_LOWER=$bbDownName;
            PERIOD=$period;
            MA_TYPE=$maType;
            DEV_UP=$udev;
            DEV_DOWN=$ddev";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=ARITHMETIC;
            METHOD=SUB;
            OUTPUT=$subName;
            INPUT_1=$bbUpName;
            INPUT_2=$bbDownName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=ARITHMETIC;
            METHOD=DIV;
            OUTPUT=$divName;
            INPUT_1=$subName;
            INPUT_2=$bbMidName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            OUTPUT=$bbwName;
            INPUT=$divName;
            STYLE=$bbwStyle;
            COLOR=$bbwColor;
            Z=$bbwZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# update chart
$command = "COMMAND=CHART_UPDATE;
            CHART=$chartName;
            DATE=$dateName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
