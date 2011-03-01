# qtstalker Bollinger Bandwidth indicator

$bbUpName = 'BB+';
$bbMidName = 'BB';
$bbDownName = 'BB-';
$period = 20;
$maType = 'SMA';
$udev = 2;
$ddev = 2;

$closeName = 'close';
$subName = 'sub';

$bwName = 'BW';
$bwColor = 'red';
$bwStyle = 'Line';
$bwZ = '0';

###################################################################

$|++;

$command = "PLUGIN=CLOSE,NAME=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=BBANDS,INPUT=$closeName,NAME_UPPER=$bbUpName,NAME_MIDDLE=$bbMidName,NAME_LOWER=$bbDownName,PERIOD=$period,MA_TYPE=$maType,DEV_UP=$udev,DEV_DOWN=$ddev";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=SUB,NAME=$subName,INPUT=$bbUpName,INPUT2=$bbDownName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=DIV,NAME=$bwName,INPUT=$subName,INPUT2=$bbMidName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$bwName,STYLE=$bwStyle,COLOR=$bwColor,Z=$bwZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
