# delete symbols from the database

$|=1;

$command = "PLUGIN=SYMBOL_DIALOG,FLAG=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the symbols string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SYMBOL_DIALOG_SYMBOLS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

my @symbols = split(';', $rc);

$command = "PLUGIN=QUOTE_DATABASE,METHOD=TRANSACTION";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

foreach $symbol (@symbols)
{
  my @tlist = split(':', $symbol);

  $command = "PLUGIN=QUOTE_DATABASE,METHOD=DELETE,EXCHANGE=$tlist[0],SYMBOL=$tlist[1]";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command;}
}

$command = "PLUGIN=QUOTE_DATABASE,METHOD=COMMIT";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

$command = "PLUGIN=CHART_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
