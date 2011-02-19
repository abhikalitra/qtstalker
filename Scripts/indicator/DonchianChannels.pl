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

###################################################################

$|++;

# OHLC bars

$command = "PLUGIN=OHLC,NAME=$barsName,COLOR_UP=$barsUpColor,COLOR_DOWN=$barsDownColor,COLOR_NEUTRAL=$barsNeutralColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the OHLC bars
$command = "PLUGIN=INDICATOR_PLOT,NAME=$barsName,Z=$barsZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the high
$command = "PLUGIN=HIGH,NAME=high";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the index ranges for the high
$command = "PLUGIN=INDICATOR_PLOT_INDEX_RANGE,NAME=high";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the index end
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_PLOT_INDEX_RANGE_END";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# remove the most recent bar from the high
$command = "PLUGIN=INDICATOR_PLOT_INDEX_DELETE,NAME=high,INDEX=$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the low
$command = "PLUGIN=LOW,NAME=low";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the index ranges for the low
$command = "PLUGIN=INDICATOR_PLOT_INDEX_RANGE,NAME=low";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the index end
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_PLOT_INDEX_RANGE_END";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# remove the most recent bar from the low
$command = "PLUGIN=INDICATOR_PLOT_INDEX_DELETE,NAME=low,INDEX=$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the MAX of the high
$command = "PLUGIN=MAX,NAME=max,INPUT=high,PERIOD=$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the MIN of the low
$command = "PLUGIN=MIN,NAME=min,INPUT=low,PERIOD=$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# shift upper values forward 1 period
$command = "PLUGIN=INDICATOR_PLOT_INDEX_SHIFT,NAME=$upperName,INPUT=max,PERIOD=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# shift lower values forward 1 period
$command = "PLUGIN=INDICATOR_PLOT_INDEX_SHIFT,NAME=$lowerName,INPUT=min,PERIOD=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the upper band
$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$upperName,STYLE=Line,COLOR=$upperColor,Z=$upperZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the lower band
$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$lowerName,STYLE=Line,COLOR=$lowerColor,Z=$lowerZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the middle band
# get the median of the upper and lower bands
$command = "PLUGIN=MEDIAN_PRICE,NAME=$midName,INPUT=$upperName,INPUT2=$lowerName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the middle band
$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$midName,STYLE=Line,COLOR=$midColor,Z=$midZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
