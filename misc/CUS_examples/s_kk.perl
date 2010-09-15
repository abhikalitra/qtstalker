# qtstalker s_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

# yellow : Piercing Line
# orange : Dark Cloud Cover

# FIXME: Not yet dealing with "Penetration" parameter.

$|++;

# get Kirikomi
print STDOUT "INDICATOR,PLUGIN,CANDLES,PIERCING,Piercing Line,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set the plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,Piercing Line,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set the color
print STDOUT "INDICATOR,SET_COLOR_ALL,Piercing Line,yellow";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# plot it
print STDOUT "INDICATOR,SET_PLOT,Piercing Line,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }


# get Kabuse
print STDOUT "INDICATOR,PLUGIN,CANDLES,DARKCLOUDCOVER,Dark Cloud Cover,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set the plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,Dark Cloud Cover,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set the color
print STDOUT "INDICATOR,SET_COLOR_ALL,Dark Cloud Cover,orange";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# plot it
print STDOUT "INDICATOR,SET_PLOT,Dark Cloud Cover,1";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

