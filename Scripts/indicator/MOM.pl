# qtstalker MOM indicator

$input = 'c';
$momName = 'MOM';
$momStyle = 'Histogram Bar';
$momColor = 'yellow';
$momPeriod = 10;
$upColor = 'green';
$downColor = 'red';

###################################################################

$|++;

$command = "PLUGIN=CLOSE,NAME=$input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MOM,NAME=$momName,INPUT=$input,PERIOD=$momPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$momName,STYLE=$momStyle,COLOR=$momColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$momName,OP=>,VALUE=0,NAME2=$momName,COLOR=$upColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$momName,OP=<,VALUE=0,NAME2=$momName,COLOR=$downColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
