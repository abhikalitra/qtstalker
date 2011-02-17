# qtstalker s_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

# yellow : Piercing Line
# orange : Dark Cloud Cover

$pName = 'Piercing Line';
$pStyle = 'Histogram Bar';
$pColor = 'yellow';

$dccName = 'Dark Cloud Cover';
$dccStyle = 'Histogram Bar';
$dccColor = 'orange';

$penetration = '0'; # Use 50 for more reliable half-way penetration.

###################################################################

$|++;

$command = "PLUGIN=CANDLE_PATTERN,METHOD=PIERCING,PENETRATION=$penetration,NAME=$pName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$pName,STYLE=$pStyle,COLOR=$pColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CANDLE_PATTERN,METHOD=DARKCLOUDCOVER,PENETRATION=$penetration,NAME=$dccName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$dccName,STYLE=$dccStyle,COLOR=$dccColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
