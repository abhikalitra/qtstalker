# qtstalker CandleRelative indicator
# Coloured relative to previous Close.

$|++;

# get the candles
$command = "BARS,CANDLES,candles,green,red,blue";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,candles,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
