# qtstalker VolumeRelative indicator
# Coloured relative to previous Close.

$|++;

# get today's Volume
print STDOUT "REF,volume,Volume,0";
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

print STDOUT "COLOR,colorUp,relUp,volume,1,green";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COLOR,colorDown,relDown,volume,1,red";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "COLOR,colorEqual,relEqual,volume,1,blue";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,volume,Volume,yellow,Histogram Bar";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
