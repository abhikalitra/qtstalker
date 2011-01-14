# qtstalker PO indicator

$input = 'c';
$poName = 'PO';
$poStyle = 'Histogram Bar';
$poColor = 'yellow';
$method = 'APO';
$fastPeriod = 12;
$slowPeriod = 26;
$maType = 'EMA';

###################################################################

$|++;

$command = "BARS,Close,$input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PO,$method,$poName,$input,$fastPeriod,$slowPeriod,$maType";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$poName,$poStyle,$poColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
