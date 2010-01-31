# qtstalker s-kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

# yellow : Piercing Line
# orange : Dark Cloud Cover

$|++;

# get Kirikomi
print STDOUT "INDICATOR,CDL,c1,PIERCING,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,c1,s-kk-1,yellow,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Kabuse
print STDOUT "INDICATOR,CDL,c2,DARKCLOUDCOVER,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,c2,s-kk-2,orange,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
