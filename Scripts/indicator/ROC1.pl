# qtstalker ROC PERIOD=1 indicator

$input = 'c';
$rocName = 'ROC';
$rocStyle = 'Histogram Bar';
$rocColor = 'yellow';
$rocPeriod = 1;
$method = 'ROC';
$upColor = 'green';
$downColor = 'red';

###################################################################

$|++;

$command = "PLUGIN=DOHLCVI,METHOD=C,NAME=$input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=ROC,METHOD=$method,NAME=$rocName,INPUT=$input,PERIOD=$rocPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$rocName,STYLE=$rocStyle,COLOR=$rocColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$rocName,OP=GT,VALUE=0,NAME2=$rocName,COLOR=$upColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$rocName,OP=LT,VALUE=0,NAME2=$rocName,COLOR=$downColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
