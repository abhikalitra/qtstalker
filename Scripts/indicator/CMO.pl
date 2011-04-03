# qtstalker CMO indicator

$closeName = 'c';

$cmoName = 'CMO';
$cmoStyle = 'Line';
$cmoColor = 'red';
$cmoPeriod = 20;
$upColor = 'green';
$downColor = 'red';

$ma1Name = 'CMO_MA';
$ma1Period = '9';
$ma1Color = 'yellow';
$ma1Style = 'Line';

$refUpColor = 'red';
$refUp = 50;
$refDownColor = 'green';
$refDown = -50;

########################################################################

$|++;

$command = "PLUGIN=DOHLCVI,METHOD=C,NAME=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CMO,NAME=$cmoName,INPUT=$closeName,PERIOD=$cmoPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$cmoName,STYLE=$cmoStyle,COLOR=$cmoColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$cmoName,OP=GT,VALUE=0,NAME2=$cmoName,COLOR=$upColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$cmoName,OP=LT,VALUE=0,NAME2=$cmoName,COLOR=$downColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MA,METHOD=EMA,NAME=$ma1Name,INPUT=$cmoName,PERIOD=$ma1Period";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$ma1Name,STYLE=$ma1Style,COLOR=$ma1Color,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=HLINE,TYPE=RO,PRICE=$refDown,COLOR=$refDownColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=HLINE,TYPE=RO,PRICE=$refUp,COLOR=$refUpColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
