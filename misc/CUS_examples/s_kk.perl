# qtstalker s_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

# yellow : Piercing Line
# orange : Dark Cloud Cover

$|++;

# get Kirikomi
print STDOUT "INDICATOR,PLUGIN,CANDLES,PIERCING,s-kk-1,0,Histogram Bar,yellow";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,s-kk-1";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Kabuse
print STDOUT "INDICATOR,PLUGIN,CANDLES,DARKCLOUDCOVER,s-kk-2,0,Histogram Bar,orange";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,s-kk-2";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

