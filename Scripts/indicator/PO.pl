# qtstalker PO indicator

$input = 'c';
$poName = 'PO';
$poStyle = 'Histogram Bar';
$poColor = 'yellow';
$method = 'APO';
$fastPeriod = 12;
$slowPeriod = 26;
$maType = 'EMA';
$upColor = 'green';
$downColor = 'red';

###################################################################

$|++;

$command = "PLUGIN=DOHLCVI,METHOD=C,NAME=$input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=PO,METHOD=$method,NAME=$poName,INPUT=$input,PERIOD_FAST=$fastPeriod,PERIOD_SLOW=$slowPeriod,MA_TYPE=$maType";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$poName,STYLE=$poStyle,COLOR=$poColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$poName,OP=GT,VALUE=0,NAME2=$poName,COLOR=$upColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$poName,OP=LT,VALUE=0,NAME2=$poName,COLOR=$downColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
