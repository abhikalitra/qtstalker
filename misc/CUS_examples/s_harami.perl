# qtstalker s_harami - Candles: Harami and Harami Cross

# yellow  : Harami
# orange  : Harami Cross

$|++;

# get Harami
print STDOUT "INDICATOR,PLUGIN,CANDLES,HARAMI,c1,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Harami Cross
print STDOUT "INDICATOR,PLUGIN,CANDLES,HARAMICROSS,c2,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# color the c1 line
print STDOUT "INDICATOR,PLUGIN,COLOR,All,c1,yellow";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

print STDOUT "PLOT,c1,s-harami-1,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# color the c2 line
print STDOUT "INDICATOR,PLUGIN,COLOR,All,c2,orange";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

print STDOUT "PLOT,c2,s-harami-2,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
