# qtstalker ROC indicator

$input = 'c';

$rocName = 'ROC';
$rocStyle = 'Histogram Bar';
$rocColor = 'yellow';
$rocPeriod = 10;
$method = 'ROC';

###################################################################

$|++;

$command = "BARS,Close,$input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "ROC,$method,$rocName,$input,$rocPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$rocName,$rocStyle,$rocColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
