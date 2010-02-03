# qtstalker c_harami - Candles: Harami and Harami Cross

# green   : Bull Harami
# cyan    : Bull Harami Cross
# red     : Bear Harami
# magenta : Bear Harami Cross

$|++;

# get the Candles
print STDOUT "INDICATOR,CANDLES,candles,dimgray,<NONE>,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Harami
print STDOUT "INDICATOR,CANDLES,ch1,dimgray,HARAMI,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COMPARE2,c1,ch1,1,>=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COLOR,c1,candles,1,green";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COMPARE2,c2,ch1,-1,<=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COLOR,c2,candles,1,red";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Harami Cross
print STDOUT "INDICATOR,CANDLES,ch2,dimgray,HARAMICROSS,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COMPARE2,c3,ch2,1,>=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COLOR,c3,candles,1,cyan";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COMPARE2,c4,ch2,-1,<=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COLOR,c4,candles,1,magenta";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,candles,Candle,dimgray,Candle";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
