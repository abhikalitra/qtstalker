# qtstalker BARS (OHLC Bars) indicator

$name = 'Bars';

###################################################################

$|++;

$command = "BARS,BARS,$name,green,red,blue";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,$name,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
