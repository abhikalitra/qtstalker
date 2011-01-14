# display message dialog if any symbol hits a price target

$exchange = "XNYS";
$symbol = "AAPL";
$target = 320;
$length = "D";
$range = "1 Day";

######################################################################

use DateTime;

$|=1;

$command = "SYMBOL,symbol,$exchange,$symbol,$length,$range";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_INDEX_GET,symbol.0";
print STDOUT $command;
$price = <STDIN>; chomp($price); if ($price eq "ERROR") { print STDERR $command; exit; }

if ($price < $target) { exit; }

$command = "NAME";
print STDOUT $command;
$name = <STDIN>; chomp($name); if ($name eq "ERROR") { print STDERR $command; exit; }

$command = "SCRIPT_DATABASE,SAVE,$name,MINUTES,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

my $dt = DateTime->now();

$command = "MESSAGE_DIALOG,$exchange:$symbol Alert,$exchange:$symbol\nHas hit $target\n$dt";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
