# qtstalker RSI indicator

$|++;

$command = "BARS,Close,c";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "RSI,RSI,c,14";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,30,green";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "CHART_OBJECT_HLINE,RO,70,red";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_STYLE,RSI,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_COLOR,ALL,RSI,red";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT,RSI,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
