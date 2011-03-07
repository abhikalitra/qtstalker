########################################################################################################
# qtstalker Best_MA backtest. this script optimzies each MA type and period to find the best
# MA performance for a selected symbol. The test is a simple buy when the close goes above the buy MA
# and sell when the close goes below the sell MA
########################################################################################################

$dateName = 'date';
$openName = 'open';
$closeName = 'close';
$name = 'Best MA';
#$maList = 'EMA,DEMA,KAMA,SMA,TEMA,TRIMA,WMA';
$maList = 'EMA';
$barLengthList = '1;5;10;15;30;60;D;W;M';
$dateRangeList = '1 Day;1 Week;1 Month;3 Months;6 Months;1 Year;2 Years;5 Years;10 Years;25 Years;50 Years;All';

###############################################################################################################

$|++;

# display new dialog
$command = "PLUGIN=NEW_DIALOG,TITLE=Test";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the name string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=NEW_DIALOG_NAME";
print STDOUT $command;
$name = <STDIN>; chomp($name); if ($name eq "ERROR") {print STDERR $command; exit; }

# display input dialog
$s1 = "PLUGIN=INPUT_DIALOG,INPUT1_LABEL=Min Buy Period,INPUT1_TYPE=INTEGER,INPUT1_VALUE=2";
$s2 = "INPUT2_LABEL=Max Buy Period,INPUT2_TYPE=INTEGER,INPUT2_VALUE=200";
$s3 = "INPUT3_LABEL=Bar Length,INPUT3_TYPE=LIST,INPUT3_VALUE=$barLengthList";
$s4 = "INPUT4_LABEL=Date Range,INPUT4_TYPE=LIST,INPUT4_VALUE=$dateRangeList";
$command = "$s1,$s2,$s3,$s4";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the minimum buy period string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT1_DATA";
print STDOUT $command;
$minPeriod = <STDIN>; chomp($minPeriod); if ($minPeriod eq "ERROR") {print STDERR $command; exit; }

# get the maximum buy period string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT2_DATA";
print STDOUT $command;
$maxPeriod = <STDIN>; chomp($maxPeriod); if ($maxPeriod eq "ERROR") {print STDERR $command; exit; }

# get the bar length
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT3_DATA";
print STDOUT $command;
$length = <STDIN>; chomp($length); if ($length eq "ERROR") {print STDERR $command; exit; }

# get the date range
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT4_DATA";
print STDOUT $command;
$range = <STDIN>; chomp($range); if ($range eq "ERROR") {print STDERR $command; exit; }

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
  for ($loop = $minPeriod; $loop <= $maxPeriod; $loop++)
  {
    # delete the ma's
    $command = "PLUGIN=INDICATOR_PLOT_DELETE,NAME=$loop";
    print STDOUT $command;
    $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }

    # create the MA's
    $command = "PLUGIN=MA,NAME=$loop,METHOD=$maType,INPUT=$closeName,PERIOD=$loop";
    print STDOUT $command;
    $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }
  }

  for ($loop = $minPeriod; $loop <= $maxPeriod; $loop++)
  {
    for ($loop2 = $minPeriod; $loop2 <= $maxPeriod; $loop2++)
    {
      # do the test
      $s1 = "PLUGIN=TEST,NAME=$name,TAG=$loop-$loop2,SYMBOL=$exchange:$symbol";
      $s2 = "ENTER_LONG_NAME=$closeName,ENTER_LONG_OP=GT,ENTER_LONG_NAME2=$loop";
      $s3 = "EXIT_LONG_NAME=$closeName,EXIT_LONG_OP=LT,EXIT_LONG_NAME2=$loop2";
      $s4 = "DATE_NAME=$dateName,BUY_NAME=$openName,SELL_NAME=$closeName,CLOSE_NAME=$closeName";
      $command = "$s1,$s2,$s3,$s4";
      print STDOUT $command;
      $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }

      print STDERR "$name: $maType $loop $loop2";
    }
  }
}
