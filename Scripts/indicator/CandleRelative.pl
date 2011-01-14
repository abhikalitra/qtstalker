# qtstalker CandleRelative indicator
# Coloured relative to previous Close.

$name = 'Candles';

###################################################################

$|++;

# get the candles
$command = "BARS,CANDLES,$name,green,red,blue";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,$name,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
