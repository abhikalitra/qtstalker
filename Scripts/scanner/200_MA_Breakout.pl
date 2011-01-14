# scans selected symbols for any close breaking above its 200 MA
# a group is created that contains all matches

$group = "200_MA_Breakout";
$length = "D";
$range = "1 Year";
$result = "";

######################################################################

$|=1;

$command = "SYMBOL_DIALOG,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

my @symbols = split(',', $rc);

foreach $item (@symbols)
{
  $command = "CLEAR";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  my @symbol = split(':', $item);

  $command = "SYMBOL,SYMBOL,symbol,$symbol[0],$symbol[1],$length,$range";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  $command = "MA,EMA,ma,symbol,200";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  $command = "INDICATOR_PLOT_INDEX_GET,symbol.0";
  print STDOUT $command;
  $price = <STDIN>; chomp($price); if ($price eq "ERROR") { print STDERR $command; next; }

  $command = "INDICATOR_PLOT_INDEX_GET,ma.0";
  print STDOUT $command;
  $ma = <STDIN>; chomp($ma); if ($ma eq "ERROR") { print STDERR $command; next; }

  if ($price <= $ma) { next; }

  $result = $result . ",$item";
}

$command = "GROUP_DATABASE,SAVE_ALL,$group,$result";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "GROUP_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
