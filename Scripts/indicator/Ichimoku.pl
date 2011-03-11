# qtstalker Ichimoku indicator

$candleName = 'Candles';
$candleUpColor = 'green';
$candleDownColor = 'green';
$candleNeutralColor = 'green';
$candleZ = '0';

$tsName = 'TENKAN SEN';
$tsStyle = 'Line';
$tsColor = 'red';
$tsPeriod = '9';
$tsHighName = 'tsHigh';
$tsLowName = 'tsLow';
$tsZ = '1';

$ksName = 'KIJUN SEN';
$ksStyle = 'Line';
$ksColor = 'blue';
$ksPeriod = '26';
$ksHighName = 'ksHigh';
$ksLowName = 'ksLow';
$ksZ = '2';

$csName = 'CHIKOU SPAN';
$csStyle = 'Line';
$csColor = 'purple';
$csPeriod = '-26';
$csZ = '3';

$ssaName = 'SENKOU SPAN A';
$ssaStyle = 'Line';
$ssaColor = 'yellow';
$ssaPeriod = '26';
$ssaZ = '4';

$ssbName = 'SENKOU SPAN B';
$ssbStyle = 'Line';
$ssbColor = 'yellow';
$ssbPeriod = '52';
$ssbShift = '26';
$ssbZ = '5';
$ssbHighName = 'ssbHigh';
$ssbLowName = 'ssbLow';

$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';

###################################################################

$|++;

# candles

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

$command = "PLUGIN=CANDLES,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName,NAME=$candleName,COLOR_UP=$candleUpColor,COLOR_DOWN=$candleDownColor,COLOR_NEUTRAL=$candleNeutralColor";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT,NAME=$candleName,Z=$candleZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# TENKAN SEN

$command = "PLUGIN=MAX,NAME=$tsHighName,INPUT=$highName,PERIOD=$tsPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MIN,NAME=$tsLowName,INPUT=$lowName,PERIOD=$tsPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MEDIAN_PRICE,NAME=$tsName,INPUT=$tsHighName,INPUT2=$tsLowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$tsName,STYLE=$tsStyle,COLOR=$tsColor,Z=$tsZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# KIJUN SEN

$command = "PLUGIN=MAX,NAME=$ksHighName,INPUT=$highName,PERIOD=$ksPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MIN,NAME=$ksLowName,INPUT=$lowName,PERIOD=$ksPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MEDIAN_PRICE,NAME=$ksName,INPUT=$ksHighName,INPUT2=$ksLowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$ksName,STYLE=$ksStyle,COLOR=$ksColor,Z=$ksZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# CHIKOU SPAN

$command = "PLUGIN=INDICATOR_PLOT_INDEX_SHIFT,NAME=$csName,INPUT=$closeName,PERIOD=$csPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$csName,STYLE=$csStyle,COLOR=$csColor,Z=$csZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# SENKOU SPAN A

$command = "PLUGIN=MEDIAN_PRICE,NAME=tssa,INPUT=$tsName,INPUT2=$ksName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_INDEX_SHIFT,NAME=$ssaName,INPUT=tssa,PERIOD=$ssaPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$ssaName,STYLE=$ssaStyle,COLOR=$ssaColor,Z=$ssaZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# SENKOU SPAN B

$command = "PLUGIN=MAX,NAME=$ssbHighName,INPUT=$highName,PERIOD=$ssbPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MIN,NAME=$ssbLowName,INPUT=$lowName,PERIOD=$ssbPeriod";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=MEDIAN_PRICE,NAME=tssb,INPUT=$ssbHighName,INPUT2=$ssbLowName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_INDEX_SHIFT,NAME=$ssbName,INPUT=tssb,PERIOD=$ssbShift";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_ALL,NAME=$ssbName,STYLE=$ssbStyle,COLOR=$ssbColor,Z=$ssbZ";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
