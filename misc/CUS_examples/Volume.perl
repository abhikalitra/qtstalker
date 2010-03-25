# qtstalker Volume (Volume) indicator, red is down, blue is neutral, green is up, color based on close price.

$|++;

# get the volume data
print STDOUT "INDICATOR,PLUGIN,REF,vol,Volume,0";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# color the vol line
print STDOUT "INDICATOR,PLUGIN,COLOR,All,vol,red";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# get close data
print STDOUT "INDICATOR,PLUGIN,REF,cl,Close,0";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# get close data offset -1 (returns previous days close)
print STDOUT "INDICATOR,PLUGIN,REF,pcl,Close,1";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# return true if Close > PClose
print STDOUT "INDICATOR,PLUGIN,COMPARE,comp1,cl,pcl,>";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# color the Volume data green if comp1 == true
# we want to color up days green
print STDOUT "INDICATOR,PLUGIN,COLOR,Compare,comp1,vol,1,green";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# return true if Close == PClose
print STDOUT "INDICATOR,PLUGIN,COMPARE,comp2,cl,pcl,=";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# color the Volume data blue if comp2 == true
# we want to color neutral days blue
print STDOUT "INDICATOR,PLUGIN,COLOR,Compare,comp2,vol,1,blue";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

print STDOUT "PLOT,vol,V,Histogram Bar";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }



