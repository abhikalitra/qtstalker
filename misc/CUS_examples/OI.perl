# qtstalker OI (Open Interest) indicator

$|++;

# create the OI line
print STDOUT "INDICATOR,PLUGIN,REF,oi,OI,0";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# color the OI line
print STDOUT "INDICATOR,PLUGIN,COLOR,All,oi,yellow";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot the OI line
print STDOUT "PLOT,oi,OI,Line";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }


