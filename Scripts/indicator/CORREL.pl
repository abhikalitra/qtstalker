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

$command = "BARS,BARS,$barsName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "SYMBOL,SYMBOL,$symbolName,$exchange,$symbol,-1,-1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CORREL,$correlName,$barsName,$symbolName,$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,$refDown,$refDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,$refMid,$refMidColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,$refUp,$refUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$correlName,$correlStyle,$correlColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
