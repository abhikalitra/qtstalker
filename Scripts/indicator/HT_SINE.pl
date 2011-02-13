# qtstalker HT SINE indicator

$input = 'c';

$sineName = 'SINE';
$sineStyle = 'Line';
$sineColor = 'red';

$leadName = 'LEAD';
$leadStyle = 'Line';
$leadColor = 'yellow';

###################################################################

$|++;

$command = "PLUGIN=OHLC,NAME=$input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=HT_SINE,INPUT=$input,NAME_SINE=$sineName,NAME_LEAD=$leadName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$sineName,STYLE=$sineStyle,COLOR=$sineColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$leadName,STYLE=$leadStyle,COLOR=$leadColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
