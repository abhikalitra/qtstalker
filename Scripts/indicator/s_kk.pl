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

$command = "CANDLES,PIERCING,$pName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$pName,$pStyle,$pColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CANDLES,DARKCLOUDCOVER,$dccName,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$dccName,$dccStyle,$dccColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
