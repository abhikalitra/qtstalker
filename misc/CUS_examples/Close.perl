# qtstalker Close indicator

$|++;

print STDOUT "REF,close-0,Close,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,close-0,Close,yellow,Line";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
