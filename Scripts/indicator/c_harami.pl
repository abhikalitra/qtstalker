# qtstalker c_harami - Candles: Harami and Harami Cross

$candleName = 'candles';
$candleColor = 'dimgray';

$haramiName = 'ch1';
$haramiCrossName = 'ch2';
$bullHaramiColor = 'green';
$bullHaramiCrossColor = 'cyan';
$bearHaramiColor = 'red';
$bearHaramiCrossColor = 'magenta';

###########################################################

$|++;

$command = "BARS,CANDLES,$candleName,$candleColor,$candleColor,$candleColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CANDLES,HARAMI,$haramiName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_COLOR_COMPARE_VALUE,$haramiName,<,0,$candleName,$bearHaramiColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_COLOR_COMPARE_VALUE,$haramiName,>,0,$candleName,$bullHaramiColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CANDLES,HARAMICROSS,$haramiCrossName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_COLOR_COMPARE_VALUE,$haramiCrossName,<,0,$candleName,$bearHaramiCrossColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_COLOR_COMPARE_VALUE,$haramiCrossName,>,0,$candleName,$bullHaramiCrossColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,$candleName,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
