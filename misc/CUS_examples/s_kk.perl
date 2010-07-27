# qtstalker s_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

# yellow : Piercing Line
# orange : Dark Cloud Cover

$|++;

# get Kirikomi
print STDOUT "INDICATOR,PLUGIN,CANDLES,PIERCING,s-kk-1,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set the plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,s-kk-1,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set the color
print STDOUT "INDICATOR,SET_COLOR_ALL,s-kk-1,yellow";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# plot it
print STDOUT "INDICATOR,SET_PLOT,s-kk-1";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }


# get Kabuse
print STDOUT "INDICATOR,PLUGIN,CANDLES,DARKCLOUDCOVER,s-kk-2,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set the plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,s-kk-2,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set the color
print STDOUT "INDICATOR,SET_COLOR_ALL,s-kk-2,orange";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# plot it
print STDOUT "INDICATOR,SET_PLOT,s-kk-2";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

