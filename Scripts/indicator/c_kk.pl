# qtstalker c_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

$candleName = 'candles';
$candleColor = 'dimgray';

$ckk1Name = 'ckk1'; # Piercing Line
$ckk2Name = 'ckk2'; # Dark Cloud Cover
$ckk1Color = 'green'; # Piercing Line Color
$ckk2Color = 'cyan'; # Dark Cloud Cover Color

###########################################################

$|++;

$command = "PLUGIN=CANDLES,NAME=$candleName,COLOR_UP=$candleColor,COLOR_DOWN=$candleColor,COLOR_NEUTRAL=$candleColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CANDLE_METHOD,METHOD=PIERCING,NAME=$ckk1Name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CANDLE_METHOD,METHOD=DARKCLOUDCOVER,NAME=$ckk2Name,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$ckk1Name,OP=>,VALUE=0,NAME2=$candleName,COLOR=$ckk1Color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_COLOR_COMPARE_VALUE,NAME=$ckk2Name,OP=<,VALUE=0,NAME2=$candleName,COLOR=$ckk2Color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT,NAME=$candleName,Z=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
