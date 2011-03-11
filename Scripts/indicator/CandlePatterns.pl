# qtstalker Candle indicator
# several popular candle patterns each with their own color
# bullish and bearish versions of the same candle pattern use same color e.g. bearish harami / bullish harami
# NOTE: we dont need to set the plot type because the CANDLES plugin does it for you

$candleName = 'candles';
$candleColor = 'dimgray';
$candleZ = '0';

$engulfingName = 'ENGULF';
$engulfingColor = 'magenta';

$hammerName = 'HAMMER';
$hammerColor = 'green';

$shootStarName = 'SHOOTSTAR';
$shootStarColor = 'white';

$haramiName = 'HARAMI';
$haramiColor = 'yellow';

$piercingName = 'PIERCING';
$piercingColor = 'orange';

$dccName = 'DCC';
$dccColor = 'cyan';

$dojiName = 'DOJI';
$dojiColor = 'purple';

$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';

###########################################################

$|++;

$command = "PLUGIN=OPEN,NAME=$openName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=HIGH,NAME=$highName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=LOW,NAME=$lowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CLOSE,NAME=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CANDLES,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,NAME=$candleName,COLOR_UP=$candleColor,COLOR_DOWN=$candleColor,COLOR_NEUTRAL=$candleColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# engulfing pattern
$command = "PLUGIN=CANDLE_PATTERN,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,METHOD=ENGULFING,NAME=$engulfingName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$engulfingName,OP=NE,VALUE=0,NAME2=$candleName,COLOR=$engulfingColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# hammer pattern
$command = "PLUGIN=CANDLE_PATTERN,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,METHOD=HAMMER,NAME=$hammerName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$hammerName,OP=NE,VALUE=0,NAME2=$candleName,COLOR=$hammerColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# shooting star pattern
$command = "PLUGIN=CANDLE_PATTERN,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,METHOD=SHOOTINGSTAR,NAME=$shootStarName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$shootStarName,OP=NE,VALUE=0,NAME2=$candleName,COLOR=$shootStarColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# harami pattern
$command = "PLUGIN=CANDLE_PATTERN,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,METHOD=HARAMI,NAME=$haramiName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$haramiName,OP=NE,VALUE=0,NAME2=$candleName,COLOR=$haramiColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# piercing pattern
$command = "PLUGIN=CANDLE_PATTERN,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,METHOD=PIERCING,NAME=$piercingName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$piercingName,OP=NE,VALUE=0,NAME2=$candleName,COLOR=$piercingColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# dark cloud cover pattern
$command = "PLUGIN=CANDLE_PATTERN,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,METHOD=DARKCLOUDCOVER,NAME=$dccName,PENETRATION=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$dccName,OP=NE,VALUE=0,NAME2=$candleName,COLOR=$dccColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# doji pattern
$command = "PLUGIN=CANDLE_PATTERN,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,METHOD=DOJI,NAME=$dojiName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$dojiName,OP=NE,VALUE=0,NAME2=$candleName,COLOR=$dojiColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot candles
$command = "PLUGIN=INDICATOR_PLOT,NAME=$candleName,Z=$candleZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
