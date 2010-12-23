# qtstalker Candle indicator
# NOTE: we dont need to set the plot type because the CANDLES plugin does it for you

$|++;

# get the Candle indicator
$command = "BARS,CANDLES,candles";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set the color
$command = "INDICATOR_PLOT_COLOR,ALL,candles,green";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot it
$command = "INDICATOR_PLOT,candles,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
