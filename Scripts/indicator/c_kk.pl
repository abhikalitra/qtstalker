# qtstalker c_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

$candleName = 'candles';
$candleColor = 'dimgray';

$ckk1Name = 'ckk1'; # Piercing Line
$ckk2Name = 'ckk2'; # Dark Cloud Cover
$ckk1Color = 'green'; # Piercing Line Color
$ckk2Color = 'cyan'; # Dark Cloud Cover Color

###########################################################

$|++;

$command = "BARS,CANDLES,$candleName,$candleColor,$candleColor,$candleColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CANDLES,PIERCING,$ckk1Name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CANDLES,DARKCLOUDCOVER,$ckk2Name,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_COLOR_COMPARE_VALUE,$ckk1Name,>,0,$candleName,$ckk1Color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_COLOR_COMPARE_VALUE,$ckk2Name,<,0,$candleName,$ckk2Color";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,$candleName,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
