# qtstalker s-kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

$|++;

# get Kirikomi
print STDOUT "CDLPIERCING,c1,Candle,dimgray";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
print STDOUT "PLOT,c1,c1,yellow,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Kabuse
print STDOUT "CDLDARKCLOUDCOVER,c2,Candle,dimgray";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
print STDOUT "PLOT,c2,c2,orange,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
