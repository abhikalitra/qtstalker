# qtstalker s_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

# yellow : Piercing Line
# orange : Dark Cloud Cover

$|++;

# get Kirikomi
print STDOUT "INDICATOR,PLUGIN,CANDLES,PIERCING,Kirikomi,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set the plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,Kirikomi,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set the color
print STDOUT "INDICATOR,SET_COLOR_ALL,Kirikomi,yellow";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# plot it
print STDOUT "INDICATOR,SET_PLOT,Kirikomi";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }


# get Kabuse
print STDOUT "INDICATOR,PLUGIN,CANDLES,DARKCLOUDCOVER,Kabuse,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set the plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,Kabuse,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set the color
print STDOUT "INDICATOR,SET_COLOR_ALL,Kabuse,orange";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# plot it
print STDOUT "INDICATOR,SET_PLOT,Kabuse";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

