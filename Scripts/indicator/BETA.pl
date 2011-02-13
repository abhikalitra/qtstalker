# qtstalker BETA indicator

$barsName = 'Bars';

$betaName = 'BETA';
$betaStyle = 'Line';
$betaColor = 'red';

$symbolName = 'symbol';
$exchange = 'XNYS';
$symbol = '^GSPC';
$period = 5;

###################################################################

$|++;

$command = "PLUGIN=CLOSE,NAME=$barsName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=SYMBOL,NAME=$symbolName,EXCHANGE=$exchange,SYMBOL=$symbol,LENGTH=-1,RANGE=-1,FIELD=CLOSE";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=BETA,NAME=$betaName,INPUT=$barsName,INPUT2=$symbolName,PERIOD=$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$betaName,STYLE=$betaStyle,COLOR=$betaColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
