# qtstalker c_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

# green : Piercing Line
# red   : Dark Cloud Cover

$|++;

# get the Candles
print STDOUT "INDICATOR,PLUGIN,CANDLES,NONE,candles,dimgray";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Kirikomi
print STDOUT "INDICATOR,PLUGIN,CANDLES,PIERCING,ckk1,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,PLUGIN,COMPARE,c1,ckk1,1,>=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,PLUGIN,COLOR,Compare,c1,candles,1,green";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Kabuse
print STDOUT "INDICATOR,PLUGIN,CANDLES,DARKCLOUDCOVER,ckk2,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,PLUGIN,COMPARE,c2,ckk2,-1,<=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,PLUGIN,COLOR,Compare,c2,candles,1,red";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,candles,Candle,Candle";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
