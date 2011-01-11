# qtstalker OI (Open Interest) indicator

$|++;

$command = "BARS,OI,oi";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_STYLE,oi,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_COLOR,ALL,oi,yellow";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,oi,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
