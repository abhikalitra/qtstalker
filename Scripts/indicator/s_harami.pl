# qtstalker s_harami - Candles: Harami and Harami Cross

# yellow  : Harami
# orange  : Harami Cross
#
# Positive values are bullish, negative values are bearish.

$|++;

# get Harami
$command = "CANDLES,HARAMI,Harami";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get Harami Cross
$command = "CANDLES,HARAMICROSS,Harami Cross";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set harami plot style
$command = "INDICATOR_PLOT_STYLE,Harami,Histogram Bar";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

#set the harami color
$command = "INDICATOR_PLOT_COLOR,ALL,Harami,yellow";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the harami 
$command = "INDICATOR_PLOT,Harami,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set harami cross plot style
$command = "INDICATOR_PLOT_STYLE,Harami Cross,Histogram Bar";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

#set the harami cross color
$command = "INDICATOR_PLOT_COLOR,ALL,Harami Cross,orange";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the harami cross
$command = "INDICATOR_PLOT,Harami Cross,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
