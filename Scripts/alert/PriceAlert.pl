# display message dialog if any symbol hits a price target

$exchange = "XNYS";
$symbol = "AAPL";
$target = 320;
$length = "D";
$range = "1 Day";

######################################################################

use DateTime;

$|=1;

$command = "PLUGIN=SYMBOL,NAME=symbol,EXCHANGE=$exchange,SYMBOL=$symbol,LENGTH=$length,RANGE=$range";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=INDICATOR_PLOT_INDEX_OFFSET,INDEX=symbol.0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# return the price string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_PLOT_INDEX_OFFSET_VALUE";
print STDOUT $command;
$price = <STDIN>; chomp($price); if ($price eq "ERROR") { print STDERR $command; next; }

if ($price < $target) { exit; }

$command = "PLUGIN=SCRIPT_NAME";
print STDOUT $command;
$name = <STDIN>; chomp($name); if ($name eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=SCRIPT_DATABASE,METHOD=SAVE,NAME=$name,FIELD=MINUTES,DATA=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

my $dt = DateTime->now();

$command = "PLUGIN=MESSAGE_DIALOG,TITLE=$exchange:$symbol Alert,MESSAGE=$exchange:$symbol\nHas hit $target\n$dt";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
