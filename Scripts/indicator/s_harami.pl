# qtstalker s_harami - Candles: Harami and Harami Cross

# yellow  : Harami
# orange  : Harami Cross
#
# Positive values are bullish, negative values are bearish.

$haramiName = 'Harami';
$haramiStyle = 'HistogramBar';
$haramiColor = 'yellow';

$haramiCrossName = 'Harami Cross';
$haramiCrossStyle = 'HistogramBar';
$haramiCrossColor = 'orange';

$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';

###################################################################

$|++;

$command = "PLUGIN=DOHLCVI,METHOD=O,NAME_OPEN=$openName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=DOHLCVI,METHOD=H,NAME_HIGH=$highName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=DOHLCVI,METHOD=L,NAME_LOW=$lowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=DOHLCVI,METHOD=C,NAME_CLOSE=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CANDLE_PATTERN,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,METHOD=HARAMI,NAME=$haramiName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR,METHOD=PLOT_ALL,NAME=$haramiName,STYLE=$haramiStyle,COLOR=$haramiColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CANDLE_PATTERN,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,METHOD=HARAMICROSS,NAME=$haramiCrossName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR,METHOD=PLOT_ALL,NAME=$haramiCrossName,STYLE=$haramiCrossStyle,COLOR=$haramiCrossColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
