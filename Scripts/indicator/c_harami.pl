# qtstalker c_harami - Candles: Harami and Harami Cross

# green   : Bull Harami
# cyan    : Bull Harami Cross
# red     : Bear Harami
# magenta : Bear Harami Cross

$|++;

# get the Candles
$command = "BARS,CANDLES,candles";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set the color
$command = "INDICATOR_PLOT_COLOR,ALL,candles,dimgray";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get Harami
$command = "CANDLES,HARAMI,ch1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get Harami Cross
$command = "CANDLES,HARAMICROSS,ch2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set ch1 >= 1 set candles current bar color to green
$command = "INDICATOR_PLOT_COLOR,COMPARE_VALUE_ALL,ch1,=>,1,candles.0,green";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set ch1 <= -1 set candles current bar color to red
$command = "INDICATOR_PLOT_COLOR,COMPARE_VALUE_ALL,ch1,<=,-1,candles.0,red";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set ch2 >= 1 set candles current bar color to cyan
$command = "INDICATOR_PLOT_COLOR,COMPARE_VALUE_ALL,ch2,=>,1,candles.0,cyan";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set ch2 <= -1 set candles current bar color to magenta
$command = "INDICATOR_PLOT_COLOR,COMPARE_VALUE_ALL,ch2,<=,-1,candles.0,magenta";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the candles 
$command = "INDICATOR_PLOT,candles,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
