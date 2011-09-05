# qtstalker Bollinger Bands %B indicator

# general parms
$chartName = 'BB%B';
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
$subName = 'sub';
$sub2Name = 'sub2';
$divName = 'div';

$bName = '%B';
$bColor = 'yellow';
$bStyle = 'Line';
$bZ = '0';

$period = '20';
$maType = 'SMA';
$udev = '2';
$ddev = '2';

$zeroLineColor = 'white';
$zeroLine = '0';
$zeroLineZ = '1';

$oneLineColor = 'white';
$oneLine = '1';
$oneLineZ = '1';

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
            INPUT_1=$closeName;
            INPUT_2=$bbDownName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=ARITHMETIC;
            METHOD=SUB;
            OUTPUT=$sub2Name;
            INPUT_1=$bbUpName;
            INPUT_2=$bbDownName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=ARITHMETIC;
            METHOD=DIV;
            OUTPUT=$divName;
            INPUT_1=$subName;
            INPUT_2=$sub2Name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=PLOT_LINE;
            CHART=$chartName;
            OUTPUT=$bName;
            INPUT=$divName;
            STYLE=$bStyle;
            COLOR=$bColor;
            Z=$bZ;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=CHART_OBJECT_HLINE;
            CHART=$chartName;
            PRICE=$oneLine;
            COLOR=$oneLineColor;
            Z=$oneLineZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "COMMAND=CHART_OBJECT_HLINE;
            CHART=$chartName;
            PRICE=$zeroLine;
            COLOR=$zeroLineColor;
            Z=$zeroLineZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# update chart
$command = "COMMAND=CHART_UPDATE;
            CHART=$chartName;
            DATE=$dateName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
