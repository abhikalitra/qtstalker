# scans selected symbols for any close breaking above its 200 MA
# a group is created that contains all matches

$group = "200_MA_Breakout";
$length = "D";
$range = "1 Year";
$result = "";

######################################################################

$|=1;

$command = "PLUGIN=SYMBOL_DIALOG,FLAG=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# return the symbols string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SYMBOL_DIALOG_SYMBOLS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

my @symbols = split(';', $rc);

foreach $item (@symbols)
{
  $command = "PLUGIN=INDICATOR_CLEAR";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  my @symbol = split(':', $item);

  $command = "PLUGIN=SYMBOL,NAME=symbol,EXCHANGE=$symbol[0],SYMBOL=$symbol[1],LENGTH=$length,RANGE=$range";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  $command = "PLUGIN=MA,METHOD=EMA,NAME=ma,INPUT=symbol,PERIOD=200";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  $command = "PLUGIN=INDICATOR_PLOT_INDEX_OFFSET,INDEX=symbol.0";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  # return the price string
  $command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_PLOT_INDEX_OFFSET_VALUE";
  print STDOUT $command;
  $price = <STDIN>; chomp($price); if ($price eq "ERROR") { print STDERR $command; next; }

  $command = "PLUGIN=INDICATOR_PLOT_INDEX_OFFSET,INDEX=ma.0";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  # return the ma string
  $command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_PLOT_INDEX_OFFSET_VALUE";
  print STDOUT $command;
  $ma = <STDIN>; chomp($ma); if ($ma eq "ERROR") { print STDERR $command; next; }

  if ($price <= $ma) { next; }

  $result = $result . ";$item";
}

$command = "PLUGIN=GROUP_DATABASE,METHOD=SAVE_ALL,NAME=$group,ITEMS=$result";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=GROUP_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
