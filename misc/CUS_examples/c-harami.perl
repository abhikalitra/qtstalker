# qtstalker s-harami - Candles: Harami and Harami Cross

$|++;

# get the Candles
print STDOUT "CANDLES,candles,dimgray";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Harami
print STDOUT "CDLHARAMI,ch1,Candle,white";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COMPARE2,c1,ch1,1,>=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COLOR,c1,candles,1,green";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COMPARE2,c2,ch1,-1,<=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COLOR,c2,candles,1,red";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Harami Cross
print STDOUT "CDLHARAMICROSS,ch2,Candle,white";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COMPARE2,c3,ch2,1,>=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COLOR,c3,candles,1,cyan";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COMPARE2,c4,ch2,-1,<=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COLOR,c4,candles,1,magenta";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,candles,Candle,dimgray,Candle";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
