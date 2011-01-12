# qtstalker c_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

# green : Piercing Line
# red   : Dark Cloud Cover

$|++;

$command = "BARS,CANDLES,candles,dimgray,dimgray,dimgray";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CANDLES,PIERCING,ckk1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CANDLES,DARKCLOUDCOVER,ckk2,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set ckk1 >= 1 set candles current bar color to green
$command = "INDICATOR_PLOT_COLOR,COMPARE_VALUE_ALL,ckk1,=>,1,candles.0,green";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set ckk2 <= -1 set candles current bar color to red
$command = "INDICATOR_PLOT_COLOR,COMPARE_VALUE_ALL,ckk2,<=,-1,candles.0,red";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,candles,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
