# qtstalker s-harami - Candles: Harami and Harami Cross

# yellow  : Harami
# orange  : Harami Cross

$|++;

# get Harami
print STDOUT "INDICATOR,CDL,c1,HARAMI,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Harami Cross
print STDOUT "INDICATOR,CDL,c2,HARAMICROSS,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,c1,s-harami-1,yellow,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,c2,s-harami-2,orange,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
