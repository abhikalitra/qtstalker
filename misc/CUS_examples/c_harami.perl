# qtstalker c_harami - Candles: Harami and Harami Cross

# green   : Bull Harami
# cyan    : Bull Harami Cross
# red     : Bear Harami
# magenta : Bear Harami Cross

$|++;

# get the Candles
print STDOUT "INDICATOR,PLUGIN,CANDLES,NONE,candles,dimgray";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Harami
print STDOUT "INDICATOR,PLUGIN,CANDLES,HARAMI,ch1,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,PLUGIN,COMPARE,c1,ch1,1,>=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,PLUGIN,COLOR,Compare,c1,candles,1,green";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

#print STDOUT "INDICATOR,PLUGIN,COMPARE,c2,ch1,-1,<=";
#$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

#print STDOUT "INDICATOR,PLUGIN,COLOR,Compare,c2,candles,1,red";
#$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Harami Cross
print STDOUT "INDICATOR,PLUGIN,CANDLES,HARAMICROSS,ch2,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,PLUGIN,COMPARE,c3,ch2,1,>=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,PLUGIN,COLOR,Compare,c3,candles,1,cyan";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

#print STDOUT "INDICATOR,PLUGIN,COMPARE,c4,ch2,-1,<=";
#$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

#print STDOUT "INDICATOR,PLUGIN,COLOR,Compare,c4,candles,1,magenta";
#$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,candles,Candle,Candle";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
