# qtstalker s-harami - Candles: Harami and Harami Cross

# yellow  : Harami
# orange  : Harami Cross

$|++;

# get Harami
print STDOUT "INDICATOR,CDLHARAMI,c1,Candle";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Harami Cross
print STDOUT "INDICATOR,CDLHARAMICROSS,c2,Candle";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,c1,s-harami-1,yellow,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,c2,s-harami-2,orange,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
