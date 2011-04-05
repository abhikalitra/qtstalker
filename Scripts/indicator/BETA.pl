# qtstalker BETA indicator

$barsName = 'Bars';

$betaName = 'BETA';
$betaStyle = 'HistogramBar';
$betaColor = 'red';

$symbolName = 'symbol';
$exchange = 'XNYS';
$symbol = '^GSPC';
$period = 5;

$upColor = 'green';
$downColor = 'red';

###################################################################

$|++;

$command = "PLUGIN=DOHLCVI,METHOD=C,NAME=$barsName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=QUOTE_DATABASE_GET,NAME_CLOSE=$symbolName,EXCHANGE=$exchange,SYMBOL=$symbol,LENGTH=-1,RANGE=-1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=BETA,NAME=$betaName,INPUT=$barsName,INPUT2=$symbolName,PERIOD=$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$betaName,STYLE=$betaStyle,COLOR=$betaColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$betaName,OP=GT,VALUE=0,NAME2=$betaName,COLOR=$upColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$betaName,OP=LT,VALUE=0,NAME2=$betaName,COLOR=$downColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
