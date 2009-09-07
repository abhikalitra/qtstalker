# qtstalker Close indicator

$|++;

print STDOUT "REF,close_0,Close,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,close_0,Close,yellow,Line";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
