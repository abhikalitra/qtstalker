# qtstalker HT indicator

$barsName = 'Bars';

$name = 'HT';
$style = 'Line';
$color = 'yellow';
$method = 'TRENDLINE';

###################################################################

$|++;

$command = "BARS,BARS,$barsName,green,red,blue";
print STDOUT $command;
$a = <STDIN>; chomp($a); if ($a eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,$barsName,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "HT,$method,$name,$barsName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$name,$style,$color,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
