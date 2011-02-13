# qtstalker CORREL indicator

$barsName = 'Bars';

$correlName = 'CORREL';
$correlStyle = 'Line';
$correlColor = 'red';

$symbolName = 'symbol';
$exchange = 'XNYS';
$symbol = '^GSPC';
$period = 30;

$refUpColor = 'white';
$refUp = 1;

$refMidColor = 'white';
$refMidDown = 0;

$refDownColor = 'white';
$refDown = -1;

###################################################################

$|++;

$command = "PLUGIN=CLOSE,NAME=$barsName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=SYMBOL,NAME=$symbolName,EXCHANGE=$exchange,SYMBOL=$symbol,LENGTH=-1,RANGE=-1,FIELD=CLOSE";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CORREL,NAME=$correlName,INPUT=$barsName,INPUT2=$symbolName,PERIOD=$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE_TEMP,PRICE=$refDown,COLOR=$refDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE_TEMP,PRICE=$refMid,COLOR=$refMidColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CHART_OBJECT_HLINE_TEMP,PRICE=$refUp,COLOR=$refUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$correlName,STYLE=$correlStyle,COLOR=$correlColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
