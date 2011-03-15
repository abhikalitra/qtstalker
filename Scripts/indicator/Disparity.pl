# qtstalker Disparity Index indicator
# See: http://tadoc.org/indicator/DISPARITY.htm
# dix = ((100 * (close - ma)) / close)

$closeName = 'cl';
$upColor = 'green';
$downColor = 'red';

$name = 'Disparity';
$style = 'Histogram Bar';
$color = 'yellow';
$period = 13;

$maType = 'EMA';
$maName = 'sma_13';

########################################################################

$|++;

# Get the close
$command = "PLUGIN=DOHLCVI,METHOD=C,NAME=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# Get the 13-bar MA
$command = "PLUGIN=MA,METHOD=$maType,NAME=$maName,INPUT=$closeName,PERIOD=$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# difference of close - ma
$command = "PLUGIN=SUB,NAME=sub,INPUT=$closeName,INPUT2=$maName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# copy array and fill with '100' value
$command = "PLUGIN=COPY_ARRAY,NAME=val100,INPUT=$maName,VALUE=100";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# multiply 100 * sub
$command = "PLUGIN=MULT,NAME=mult,INPUT=val100,INPUT2=sub";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# calculate disparity (mult / close)
$command = "PLUGIN=DIV,NAME=$name,INPUT=mult,INPUT2=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$name,STYLE=$style,COLOR=$color,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$name,OP=GT,VALUE=0,NAME2=$name,COLOR=$upColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$name,OP=LT,VALUE=0,NAME2=$name,COLOR=$downColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
