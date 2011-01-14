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

$command = "BARS,BARS,$barsName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "SYMBOL,SYMBOL,$symbolName,$exchange,$symbol,-1,-1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "BETA,$betaName,$barsName,$symbolName,$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$betaName,$betaStyle,$betaColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
