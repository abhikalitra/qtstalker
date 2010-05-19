# qtstalker s_harami - Candles: Harami and Harami Cross

# yellow  : Harami
# orange  : Harami Cross
#
# Positive values are bullish, negative values are bearish.

$|++;

# get Harami
print STDOUT "INDICATOR,PLUGIN,CANDLES,HARAMI,c1,0,Histogram Bar,yellow";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Harami Cross
print STDOUT "INDICATOR,PLUGIN,CANDLES,HARAMICROSS,c2,0,Histogram Bar,orange";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,c1";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,c2";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

