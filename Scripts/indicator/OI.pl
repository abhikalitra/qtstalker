# qtstalker OI (Open Interest) indicator

$oiName = 'OI';
$oiStyle = 'Line';
$oiColor = 'yellow';

###################################################################

$|++;

$command = "PLUGIN=DOHLCVI,METHOD=I,NAME=$oiName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$oiName,STYLE=$oiStyle,COLOR=$oiColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
