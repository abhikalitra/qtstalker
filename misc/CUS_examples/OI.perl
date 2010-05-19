# qtstalker OI (Open Interest) indicator

$|++;

# create the OI line
print STDOUT "INDICATOR,NEW,OI,oi,Line,yellow";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot the OI line
print STDOUT "PLOT,oi";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }


