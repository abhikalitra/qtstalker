# qtstalker CMO indicator

$closeName = 'c';

$cmoName = 'CMO';
$cmoStyle = 'Histogram Bar';
$cmoColor = 'red';
$cmoPeriod = 14;

########################################################################

$|++;

$command = "BARS,Close,$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CMO,$cmoName,$closeName,$cmoPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$cmoName,$cmoStyle,$cmoColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
