# qtstalker c-kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

$|++;

# get the Candles
print STDOUT "CANDLES,candles,dimgray";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Kirikomi
print STDOUT "CDLPIERCING,ckk1,Candle,white";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COMPARE2,c1,ckk1,1,>=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COLOR,color1,c1,candles,1,green";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Kabuse
print STDOUT "CDLDARKCLOUDCOVER,ckk2,Candle,white";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COMPARE2,c2,ckk2,-1,<=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COLOR,color2,c2,candles,1,red";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,candles,Candle,dimgray,Candle";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
