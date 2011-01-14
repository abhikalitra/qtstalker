# qtstalker Candle indicator
# NOTE: we dont need to set the plot type because the CANDLES plugin does it for you

$name = 'Candles';

###################################################################

$|++;

$command = "BARS,CANDLES,$name,green,green,green";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,$name,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
