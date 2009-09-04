# qtstalker CandleRelative indicator
# Coloured relative to previous Close.

$|++;

# get the Candles
print STDOUT "CANDLES,candles,dimgray";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get today's Close
print STDOUT "REF,close_0,Close,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get yesterday's Close
print STDOUT "REF,close_1,Close,1";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# close is relatively up
print STDOUT "COMPARE,relUp,close_0,close_1,>";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# close is relatively down
print STDOUT "COMPARE,relDown,close_0,close_1,<";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# close is equal
print STDOUT "COMPARE,relEqual,close_0,close_1,=";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COLOR,colorUp,relUp,candles,1,green";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COLOR,colorDown,relDown,candles,1,red";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COLOR,colorEqual,relEqual,candles,1,blue";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,candles,Candle,dimgray,Candle";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
