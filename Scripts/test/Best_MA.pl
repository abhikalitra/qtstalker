# qtstalker MA backtest

$minBuyPeriod = '2';
$maxBuyPeriod = '20';
$minSellPeriod = '2';
$maxSellPeriod = '20';
$exchange = 'YAHOO';
$symbol = 'RIM.TO';
$length = 'D';
#$range = 'All';
$range = '5 Years';
$dateName = 'date';
$openName = 'open';
$closeName = 'close';
$maBuyName = 'bma';
$maSellName = 'sma';
$name = 'Best MA';
$maList = 'EMA,DEMA,KAMA,SMA,TEMA,TRIMA,WMA';

###################################################################

$|++;

# display input dialog
$s1 = "PLUGIN=INPUT_DIALOG,INPUT1_LABEL=Min Buy Period,INPUT1_TYPE=INTEGER,INPUT1_VALUE=$minBuyPeriod";
$s2 = "INPUT2_LABEL=Max Buy Period,INPUT2_TYPE=INTEGER,INPUT2_VALUE=$maxBuyPeriod";
$s3 = "INPUT3_LABEL=Min Sell Period,INPUT3_TYPE=INTEGER,INPUT3_VALUE=$minSellPeriod";
$s4 = "INPUT4_LABEL=Max Sell Period,INPUT4_TYPE=INTEGER,INPUT4_VALUE=$maxSellPeriod";
$command = "$s1,$s2,$s3,$s4";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }

# get the minimum buy period string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT1_DATA";
print STDOUT $command;
$minBuyPeriod = <STDIN>; chomp($minBuyPeriod); if ($minBuyPeriod eq "ERROR") {print STDERR $command; exit; }

# get the maximum buy period string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT2_DATA";
print STDOUT $command;
$maxBuyPeriod = <STDIN>; chomp($maxBuyPeriod); if ($maxBuyPeriod eq "ERROR") {print STDERR $command; exit; }

# get the minimum sell period string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT3_DATA";
print STDOUT $command;
$minSellPeriod = <STDIN>; chomp($minSellPeriod); if ($minSellPeriod eq "ERROR") {print STDERR $command; exit; }

# get the maximum sell period string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT4_DATA";
print STDOUT $command;
$maxSellPeriod = <STDIN>; chomp($maxSellPeriod); if ($maxSellPeriod eq "ERROR") {print STDERR $command; exit; }

# display the symbol search dialog to return symbols
$command = "PLUGIN=SYMBOL_DIALOG,FLAG=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the symbols string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SYMBOL_DIALOG_SYMBOLS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

my @symbols = split(';', $rc);
my @parts = split(':', $symbols[0]);
$exchange = $parts[0];
$symbol = $parts[1];

# get the symbol data
$command = "PLUGIN=QUOTE_DATABASE_GET,NAME_DATE=$dateName,NAME_OPEN=$openName,NAME_CLOSE=$closeName,EXCHANGE=$exchange,SYMBOL=$symbol,LENGTH=$length,RANGE=$range";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

my @maTypes = split(',', $maList);

foreach $maType (@maTypes)
{
  for ($loop = $minBuyPeriod; $loop <= $maxBuyPeriod; $loop++)
  {
    # delete the buy ma
    $command = "PLUGIN=INDICATOR_PLOT_DELETE,NAME=$maBuyName";
    print STDOUT $command;
    $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }

    # get the buy MA
    $command = "PLUGIN=MA,NAME=$maBuyName,METHOD=$maType,INPUT=$closeName,PERIOD=$loop";
    print STDOUT $command;
    $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }

    for ($loop2 = $minSellPeriod; $loop2 <= $maxSellPeriod; $loop2++)
    {
      # delete the sell ma
      $command = "PLUGIN=INDICATOR_PLOT_DELETE,NAME=$maSellName";
      print STDOUT $command;
      $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }

      # get the sell MA
      $command = "PLUGIN=MA,NAME=$maSellName,METHOD=$maType,INPUT=$closeName,PERIOD=$loop2";
      print STDOUT $command;
      $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }

      # do the test
      $s1 = "PLUGIN=TEST,NAME=$name,VERSION=$maType-$loop-$loop2,SYMBOL=$exchange:$symbol";
      $s2 = "ENTER_LONG_NAME=$closeName,ENTER_LONG_OP=GT,ENTER_LONG_NAME2=$maBuyName";
      $s3 = "EXIT_LONG_NAME=$closeName,EXIT_LONG_OP=LT,EXIT_LONG_NAME2=$maSellName";
      $s4 = "DATE_NAME=$dateName,BUY_NAME=$openName,SELL_NAME=$closeName,CLOSE_NAME=$closeName";
      $command = "$s1,$s2,$s3,$s4";
      print STDOUT $command;
      $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }

      print STDERR "$maType $loop $loop2";
    }
  }
}
