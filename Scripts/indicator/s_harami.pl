# qtstalker s_harami - Candles: Harami and Harami Cross

# yellow  : Harami
# orange  : Harami Cross
#
# Positive values are bullish, negative values are bearish.

$haramiName = 'Harami';
$haramiStyle = 'Histogram Bar';
$haramiColor = 'yellow';

$haramiCrossName = 'Harami Cross';
$haramiCrossStyle = 'Histogram Bar';
$haramiCrossColor = 'orange';

###################################################################

$|++;

$command = "PLUGIN=CANDLE_PATTERN,METHOD=HARAMI,NAME=$haramiName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$haramiName,STYLE=$haramiStyle,COLOR=$haramiColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CANDLE_PATTERN,METHOD=HARAMICROSS,NAME=$haramiCrossName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$haramiCrossName,STYLE=$haramiCrossStyle,COLOR=$haramiCrossColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
