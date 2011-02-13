# qtstalker s_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

# yellow : Piercing Line
# orange : Dark Cloud Cover

# FIXME: Not yet dealing with "Penetration" parameter.

$pName = 'Piercing Line';
$pStyle = 'Histogram Bar';
$pColor = 'yellow';

$dccName = 'Dark Cloud Cover';
$dccStyle = 'Histogram Bar';
$dccColor = 'orange';

###################################################################

$|++;

$command = "PLUGIN=CANDLE_METHOD,METHOD=PIERCING,NAME=$pName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$pName,STYLE=$pStyle,COLOR=$pColor,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CANDLE_METHOD,METHOD=DARKCLOUDCOVER,NAME=$dccName,PENETRATION=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$dccName,STYLE=$dccStyle,COLOR=$dccColor,Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
