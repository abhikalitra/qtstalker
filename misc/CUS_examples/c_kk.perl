# qtstalker c_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

# green : Piercing Line
# red   : Dark Cloud Cover

$|++;

# get the Candles
print STDOUT "INDICATOR,CANDLES,candles,dimgray,<NONE>,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Kirikomi
print STDOUT "INDICATOR,CANDLES,ckk1,dimgray,PIERCING,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COMPARE2,c1,ckk1,1,>=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COLOR,c1,candles,1,green";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Kabuse
print STDOUT "INDICATOR,CANDLES,ckk2,dimgray,DARKCLOUDCOVER,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COMPARE2,c2,ckk2,-1,<=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COLOR,c2,candles,1,red";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,candles,Candle,dimgray,Candle";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
