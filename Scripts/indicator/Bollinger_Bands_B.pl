# qtstalker Bollinger Bands %B indicator

$bbUpName = 'BB+';
$bbMidName = 'BB';
$bbDownName = 'BB-';
$closeName = 'close';
$subName = 'sub';
$sub2Name = 'sub2';

$bName = '%B';
$bColor = 'red';
$bStyle = 'Line';
$bZ = '0';

$period = 20;
$maType = 'SMA';
$udev = 2;
$ddev = 2;

$zeroLineColor = 'white';
$zeroLine = 0;

$oneLineColor = 'white';
$oneLine = 1;

###################################################################

$|++;

$command = "PLUGIN=DOHLCVI,METHOD=C,NAME_CLOSE=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=BBANDS,INPUT=$closeName,NAME_UPPER=$bbUpName,NAME_MIDDLE=$bbMidName,NAME_LOWER=$bbDownName,PERIOD=$period,MA_TYPE=$maType,DEV_UP=$udev,DEV_DOWN=$ddev";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=ARITHMETIC,METHOD=SUB,NAME=$subName,INPUT=$closeName,INPUT2=$bbDownName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=ARITHMETIC,METHOD=SUB,NAME=$sub2Name,INPUT=$bbUpName,INPUT2=$bbDownName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=ARITHMETIC,METHOD=DIV,NAME=$bName,INPUT=$subName,INPUT2=$sub2Name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR,METHOD=PLOT_ALL,NAME=$bName,STYLE=$bStyle,COLOR=$bColor,Z=$bZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=HLINE,TYPE=RO,PRICE=$oneLine,COLOR=$oneLineColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=HLINE,TYPE=RO,PRICE=$zeroLine,COLOR=$zeroLineColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
