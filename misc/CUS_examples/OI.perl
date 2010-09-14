# qtstalker OI (Open Interest) indicator

$|++;

# create the OI line
print STDOUT "INDICATOR,NEW,OI,oi";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set plot style
print STDOUT "INDICATOR,SET_PLOT_STYLE,oi,Line";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# set the color
print STDOUT "INDICATOR,SET_COLOR_ALL,oi,yellow";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot the oi
print STDOUT "INDICATOR,SET_PLOT,oi,0";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }


