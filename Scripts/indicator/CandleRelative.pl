# qtstalker CandleRelative indicator
# Coloured relative to previous Close.

$name = 'Candles';
$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';

###################################################################

$|++;

$command = "PLUGIN=DOHLCVI,METHOD=O,NAME_OPEN=$openName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=DOHLCVI,METHOD=H,NAME_HIGH=$highName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=DOHLCVI,METHOD=L,NAME_LOW=$lowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=DOHLCVI,METHOD=C,NAME_CLOSE=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the candles
$command = "PLUGIN=CANDLES,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,NAME=$name,COLOR_UP=green,COLOR_DOWN=red,COLOR_NEUTRAL=blue";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR,METHOD=PLOT,NAME=$name,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
