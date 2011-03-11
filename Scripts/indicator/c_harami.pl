# qtstalker c_harami - Candles: Harami and Harami Cross

$candleName = 'candles';
$candleColor = 'dimgray';

$haramiName = 'ch1';
$haramiCrossName = 'ch2';
$bullHaramiColor = 'green';
$bullHaramiCrossColor = 'cyan';
$bearHaramiColor = 'red';
$bearHaramiCrossColor = 'magenta';

$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';

###########################################################

$|++;

$command = "PLUGIN=CANDLES,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,NAME=$candleName,COLOR_UP=$candleColor,COLOR_DOWN=$candleColor,COLOR_NEUTRAL=$candleColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CANDLE_PATTERN,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,METHOD=HARAMI,NAME=$haramiName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$haramiName,OP=LT,VALUE=0,NAME2=$candleName,COLOR=$bearHaramiColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$haramiName,OP=GT,VALUE=0,NAME2=$candleName,COLOR=$bullHaramiColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CANDLE_PATTERN,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,METHOD=HARAMICROSS,NAME=$haramiCrossName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$haramiCrossName,OP=LT,VALUE=0,NAME2=$candleName,COLOR=$bearHaramiCrossColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$haramiCrossName,OP=GT,VALUE=0,NAME2=$candleName,COLOR=$bullHaramiCrossColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT,NAME=$candleName,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
