# scans selected symbols for latest close breaking above its 200 MA
# a group is created that contains all matches

$group = '200_MA_Breakout';
$length = 'D';
$range = '1 Year';
$result = '';

######################################################################

$|=1;

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
  $command = "PLUGIN=INDICATOR,METHOD=CLEAR";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  my @symbol = split(':', $item);

  $command = "PLUGIN=QUOTE_DATABASE,METHOD=GET,NAME_CLOSE=symbol,EXCHANGE=$symbol[0],SYMBOL=$symbol[1],LENGTH=$length,RANGE=$range";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  $command = "PLUGIN=MA,METHOD=EMA,NAME=ma,INPUT=symbol,PERIOD=200";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  $command = "PLUGIN=INDICATOR,METHOD=VALUE_GET,NAME=symbol.0";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  # return the price string
  $command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_GET_VALUE";
  print STDOUT $command;
  $price = <STDIN>; chomp($price); if ($price eq "ERROR") { print STDERR $command; next; }

  $command = "PLUGIN=INDICATOR,METHOD=VALUE_GET,NAME=ma.0";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  # return the ma string
  $command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_GET_VALUE";
  print STDOUT $command;
  $ma = <STDIN>; chomp($ma); if ($ma eq "ERROR") { print STDERR $command; next; }

  if ($price <= $ma) { next; }

  $result = $result . ";$item";
}

$command = "PLUGIN=GROUP_DATABASE,METHOD=SAVE_ALL,NAME=$group,ITEMS=$result";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "PLUGIN=CONTROL_PANEL,METHOD=GROUP_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
