# qtstalker s_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

# yellow : Piercing Line
# orange : Dark Cloud Cover

$|++;

# get Kirikomi
print STDOUT "INDICATOR,CANDLES,PIERCING,c1,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# color the c1 line
print STDOUT "INDICATOR,COLOR,All,c1,yellow";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

print STDOUT "PLOT,c1,s-kk-1,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Kabuse
print STDOUT "INDICATOR,CANDLES,DARKCLOUDCOVER,c2,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# color the c2 line
print STDOUT "INDICATOR,COLOR,All,c2,orange";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

print STDOUT "PLOT,c2,s-kk-2,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
