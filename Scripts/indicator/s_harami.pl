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

$command = "CANDLES,HARAMI,$haramiName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$haramiName,$haramiStyle,$haramiColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CANDLES,HARAMICROSS,$haramiCrossName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$haramiCrossName,$haramiCrossStyle,$haramiCrossColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
