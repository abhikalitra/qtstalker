# scans selected symbols for latest close making a new high
# a group is created that contains all matches

$result = '';
$barLengthList = '1;5;10;15;30;60;D;W;M';
$dateRangeList = '1 Day;1 Week;1 Month;3 Months;6 Months;1 Year;2 Years;5 Years;10 Years;25 Years;50 Years;All';

######################################################################

$|=1;

# display input dialog
$s1 = "PLUGIN=INPUT_DIALOG,INPUT1_LABEL=Group Name,INPUT1_TYPE=TEXT,INPUT1_VALUE=New Group";
$s2 = "INPUT2_LABEL=Bar Length,INPUT2_TYPE=LIST,INPUT2_VALUE=$barLengthList";
$s3 = "INPUT3_LABEL=Date Range,INPUT3_TYPE=LIST,INPUT3_VALUE=$dateRangeList";
$command = "$s1,$s2,$s3";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the group string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT1_DATA";
print STDOUT $command;
$group = <STDIN>; chomp($group); if ($group eq "ERROR") {print STDERR $command; exit; }

# get the bar length
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT2_DATA";
print STDOUT $command;
$length = <STDIN>; chomp($length); if ($length eq "ERROR") {print STDERR $command; exit; }

# get the date range
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT3_DATA";
print STDOUT $command;
$range = <STDIN>; chomp($range); if ($range eq "ERROR") {print STDERR $command; exit; }

# display the symbol dialog
$command = "PLUGIN=SYMBOL_DIALOG,FLAG=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# return the symbols string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SYMBOL_DIALOG_SYMBOLS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

my @symbols = split(';', $rc);

foreach $item (@symbols)
{
  # clear all arrays
  $command = "PLUGIN=INDICATOR_CLEAR";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  # split the symbol into exchange/symbol strings
  my @symbol = split(':', $item);

  # get the close bars
  $command = "PLUGIN=QUOTE_DATABASE,METHOD=GET,NAME_CLOSE=symbol,EXCHANGE=$symbol[0],SYMBOL=$symbol[1],LENGTH=$length,RANGE=$range";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  # get the most recent close price
  $command = "PLUGIN=INDICATOR_PLOT_VALUE_GET,NAME=symbol.0";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  # return the price string
  $command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_PLOT_VALUE_GET_VALUE";
  print STDOUT $command;
  $price = <STDIN>; chomp($price); if ($price eq "ERROR") { print STDERR $command; next; }

  # get the index ranges for the close
  $command = "PLUGIN=INDICATOR_PLOT_INDEX_RANGE,NAME=symbol";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  # get the index end
  $command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_PLOT_INDEX_RANGE_END";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  # remove the most recent bar from the close
  $command = "PLUGIN=INDICATOR_PLOT_INDEX_DELETE,NAME=symbol,INDEX=$rc";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  # find the highest high for the entire array
  $command = "PLUGIN=MAX,NAME=max,INPUT=symbol,PERIOD=0";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  # get the highest high from max
  $command = "PLUGIN=INDICATOR_PLOT_VALUE_GET,NAME=max.0";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  # return the highest high string
  $command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_PLOT_VALUE_GET_VALUE";
  print STDOUT $command;
  $hh = <STDIN>; chomp($hh); if ($hh eq "ERROR") { print STDERR $command; next; }

  if ($price <= $hh) { next; }

  $result = $result . ";$item";
  print STDERR "New_High $item $price $hh";
}

$command = "PLUGIN=GROUP_DATABASE,METHOD=SAVE_ALL,NAME=$group,ITEMS=$result";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=GROUP_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
