# qtstalker Candle indicator
# NOTE: we dont need to set the plot type because the CANDLES plugin does it for you

$name = 'Candles';
$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';

###################################################################

$|++;

$command = "PLUGIN=CANDLES,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,NAME=$name,COLOR_UP=green,COLOR_DOWN=green,COLOR_NEUTRAL=green";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT,NAME=$name,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
