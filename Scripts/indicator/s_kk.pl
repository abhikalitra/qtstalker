# qtstalker s_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

# yellow : Piercing Line
# orange : Dark Cloud Cover

# FIXME: Not yet dealing with "Penetration" parameter.

$|++;

# get Kirikomi
$command = "CANDLES,PIERCING,Piercing Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set the plot style
$command = "INDICATOR_PLOT_STYLE,Piercing Line,Histogram Bar";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set the color
$command = "INDICATOR_PLOT_COLOR,ALL,Piercing Line,yellow";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot it
$command = "INDICATOR_PLOT,Piercing Line,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get Kabuse
$command = "CANDLES,DARKCLOUDCOVER,Dark Cloud Cover,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set the plot style
$command = "INDICATOR_PLOT_STYLE,Dark Cloud Cover,Histogram Bar";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set the color
$command = "INDICATOR_PLOT_COLOR,ALL,Dark Cloud Cover,orange";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot it
$command = "INDICATOR_PLOT,Dark Cloud Cover,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
