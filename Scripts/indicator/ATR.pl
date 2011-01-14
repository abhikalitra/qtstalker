# qtstalker ATR (Average True Range) indicator

$name = 'ATR';
$style = 'Line';
$color = 'red';
$period = 14;

###################################################################

$|++;

$command = "ATR,$name,$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$name,$style,$color,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
