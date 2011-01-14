# qtstalker OI (Open Interest) indicator

$input = 'oi';

$oiName = 'OI';
$oiStyle = 'Line';
$oiColor = 'yellow';

###################################################################

$|++;

$command = "BARS,OI,input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$oiName,$oiStyle,$oiColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
