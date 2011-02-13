# qtstalker CandleRelative indicator
# Coloured relative to previous Close.

$name = 'Candles';

###################################################################

$|++;

# get the candles
$command = "PLUGIN=CANDLES,NAME=$name,COLOR_UP=green,COLOR_DOWN=red,COLOR_NEUTRAL=blue";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT,NAME=$name,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
