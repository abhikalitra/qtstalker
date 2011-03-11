# qtstalker FI indicator

$name = 'FI';
$style = 'Histogram Bar';
$color = 'yellow';
$period = 2;
$maType = 'EMA';
$upColor = 'green';
$downColor = 'red';
$closeName = 'Close';
$volName = 'Volume';

###################################################################

$|++;

$command = "PLUGIN=VOLUME,NAME=$volName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CLOSE,NAME=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=FI,INPUT_VOLUME=$volName,INPUT_CLOSE=$closeName,NAME=$name,PERIOD=$period,MA_TYPE=$maType";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$name,STYLE=$style,COLOR=$color,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$name,OP=GT,VALUE=0,NAME2=$name,COLOR=$upColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$name,OP=LT,VALUE=0,NAME2=$name,COLOR=$downColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
