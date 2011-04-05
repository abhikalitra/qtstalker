# qtstalker Donchian Channels indicator

$period = '20';

$barsName = 'BARS';
$barsUpColor = 'green';
$barsDownColor = 'red';
$barsNeutralColor = 'green';
$barsZ = '0';

$upperName = 'DCU';
$upperColor = 'green';
$upperZ = '1';

$midName = 'DCM';
$midColor = 'yellow';
$midZ = '2';

$lowerName = 'DCL';
$lowerColor = 'red';
$lowerZ = '3';

$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';

###################################################################

$|++;

# OHLC bars

$command = "PLUGIN=DOHLCVI,METHOD=O,NAME=$openName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=DOHLCVI,METHOD=H,NAME=$highName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=DOHLCVI,METHOD=L,NAME=$lowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=DOHLCVI,METHOD=C,NAME=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=OHLC,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,NAME=$barsName,COLOR_UP=$barsUpColor,COLOR_DOWN=$barsDownColor,COLOR_NEUTRAL=$barsNeutralColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the OHLC bars
$command = "PLUGIN=INDICATOR,METHOD=PLOT,NAME=$barsName,Z=$barsZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the index ranges for the high
$command = "PLUGIN=INDICATOR,METHOD=INDEX_RANGE,NAME=$highName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the index end
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_RANGE_END";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# remove the most recent bar from the high
$command = "PLUGIN=INDICATOR,METHOD=INDEX_DELETE,NAME=$highName,INDEX=$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the index ranges for the low
$command = "PLUGIN=INDICATOR,METHOD=INDEX_RANGE,NAME=$lowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the index end
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_RANGE_END";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# remove the most recent bar from the low
$command = "PLUGIN=INDICATOR,METHOD=INDEX_DELETE,NAME=$lowName,INDEX=$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the MAX of the high
$command = "PLUGIN=MAX,NAME=max,INPUT=$highName,PERIOD=$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the MIN of the low
$command = "PLUGIN=MIN,NAME=min,INPUT=$lowName,PERIOD=$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# shift upper values forward 1 period
$command = "PLUGIN=INDICATOR,METHOD=INDEX_SHIFT,NAME=$upperName,INPUT=max,PERIOD=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# shift lower values forward 1 period
$command = "PLUGIN=INDICATOR,METHOD=INDEX_SHIFT,NAME=$lowerName,INPUT=min,PERIOD=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the upper band
$command = "PLUGIN=INDICATOR,METHOD=PLOT_ALL,NAME=$upperName,STYLE=Line,COLOR=$upperColor,Z=$upperZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the lower band
$command = "PLUGIN=INDICATOR,METHOD=PLOT_ALL,NAME=$lowerName,STYLE=Line,COLOR=$lowerColor,Z=$lowerZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the middle band
# get the median of the upper and lower bands
$command = "PLUGIN=MEDIAN_PRICE,NAME=$midName,INPUT=$upperName,INPUT2=$lowerName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the middle band
$command = "PLUGIN=INDICATOR,METHOD=PLOT_ALL,NAME=$midName,STYLE=Line,COLOR=$midColor,Z=$midZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
