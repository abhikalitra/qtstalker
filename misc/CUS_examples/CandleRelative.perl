# qtstalker CandleRelative indicator
# Coloured relative to previous Close.

$|++;

# get the Candles
print STDOUT "INDICATOR,CANDLES,NONE,candles,dimgray";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get today's Close
print STDOUT "INDICATOR,REF,close_0,Close,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get yesterday's Close
print STDOUT "INDICATOR,REF,close_1,Close,1";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# close is relatively up
print STDOUT "INDICATOR,COMPARE,ARRAY,relUp,close_0,close_1,>";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# close is relatively down
print STDOUT "INDICATOR,COMPARE,ARRAY,relDown,close_0,close_1,<";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# close is equal
print STDOUT "INDICATOR,COMPARE,ARRAY,relEqual,close_0,close_1,=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COLOR,relUp,candles,1,green";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COLOR,relDown,candles,1,red";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "INDICATOR,COLOR,relEqual,candles,1,blue";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,candles,Candle,dimgray,Candle";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
