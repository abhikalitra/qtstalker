########################################################################################################
# qtstalker Best_MA backtest. this script optimzies each MA type and period to find the best
# MA performance for a selected symbol. The test is a simple buy when the close goes above the buy MA
# and sell when the close goes below the sell MA
########################################################################################################

$dateName = 'Date';
$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';
$avgName = 'Average';
$buyField = 'Close';
$sellField = 'Close';
$enterLongInput = 'Close';
$exitLongInput = 'Close';
$name = 'Best MA';
$maBuy = 'bma';
$maSell = 'sma';
#$maList = 'EMA,DEMA,KAMA,SMA,TEMA,TRIMA,WMA';
$maType = 'EMA';
$barLengthList = '1;5;10;15;30;60;D;W;M';
$dateRangeList = '1 Day;1 Week;1 Month;3 Months;6 Months;1 Year;2 Years;5 Years;10 Years;25 Years;50 Years;All';
$fieldList = "$openName;$highName;$lowName;$closeName";
$fieldList2 = "$openName;$closeName;$avgName";
$exchange = 'YAHOO';
$symbol = 'XIU.TO';
$equity = '50000';
$volume = '0.1';

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
$s2 = "INPUT2_LABEL=Max Buy Period,INPUT2_TYPE=INTEGER,INPUT2_VALUE=50";
$s3 = "INPUT3_LABEL=Bar Length,INPUT3_TYPE=LIST,INPUT3_VALUE=$barLengthList";
$s4 = "INPUT4_LABEL=Date Range,INPUT4_TYPE=LIST,INPUT4_VALUE=$dateRangeList";
$s5 = "INPUT5_LABEL=Buy On,INPUT5_TYPE=LIST,INPUT5_VALUE=$fieldList2";
$s6 = "INPUT6_LABEL=Sell On,INPUT6_TYPE=LIST,INPUT6_VALUE=$fieldList2";
$s7 = "INPUT7_LABEL=Equity,INPUT7_TYPE=DOUBLE,INPUT7_VALUE=$equity";
$s8 = "INPUT8_LABEL=Volume,INPUT8_TYPE=DOUBLE,INPUT8_VALUE=$volume";
$command = "$s1,$s2,$s3,$s4,$s5,$s6,$s7,$s8";
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

# get the buy field
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT5_DATA";
print STDOUT $command;
$buyField = <STDIN>; chomp($buyField); if ($buyField eq "ERROR") {print STDERR $command; exit; }

# get the sell field
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT6_DATA";
print STDOUT $command;
$sellField = <STDIN>; chomp($sellField); if ($sellField eq "ERROR") {print STDERR $command; exit; }

# get the equity field
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT7_DATA";
print STDOUT $command;
$equity = <STDIN>; chomp($equity); if ($equity eq "ERROR") {print STDERR $command; exit; }

# get the volume field
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT8_DATA";
print STDOUT $command;
$volume = <STDIN>; chomp($volume); if ($volume eq "ERROR") {print STDERR $command; exit; }

# display the symbol search dialog to return symbols
#$command = "PLUGIN=SYMBOL_DIALOG,FLAG=0";
#print STDOUT $command;
#$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the symbols string
#$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SYMBOL_DIALOG_SYMBOLS";
#print STDOUT $command;
#$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

#my @symbols = split(';', $rc);
#my @parts = split(':', $symbols[0]);
#$exchange = $parts[0];
#$symbol = $parts[1];

# get the symbol data
$command = "PLUGIN=QUOTE_DATABASE,METHOD=GET,NAME_DATE=$dateName,NAME_OPEN=$openName,NAME_HIGH=$highName,NAME_LOW=$lowName,NAME_CLOSE=$closeName,EXCHANGE=$exchange,SYMBOL=$symbol,LENGTH=$length,RANGE=$range";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the average price
$command = "PLUGIN=AVERAGE_PRICE,NAME=$avgPrice,INPUT_OPEN=$openName,INPUT_HIGH=$highName,INPUT_LOW=$lowName,INPUT_CLOSE=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

my @fields = split(';', $fieldList);

foreach $input1 (@fields)
{
  my @fields2 = split(';', $fieldList);

  foreach $input2 (@fields2)
  {
    for ($loop = $minPeriod; $loop <= $maxPeriod; $loop++)
    {
      # delete the buy ma
      $command = "PLUGIN=INDICATOR,METHOD=DELETE,NAME=buy$loop";
      print STDOUT $command;
      $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }

      # create the buy ma
      $command = "PLUGIN=MA,NAME=buy$loop,METHOD=$maType,INPUT=$input1,PERIOD=$loop";
      print STDOUT $command;
      $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }

      # delete the sell ma
      $command = "PLUGIN=INDICATOR,METHOD=DELETE,NAME=sell$loop";
      print STDOUT $command;
      $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }

      # create the sell ma
      $command = "PLUGIN=MA,NAME=sell$loop,METHOD=$maType,INPUT=$input2,PERIOD=$loop";
      print STDOUT $command;
      $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }
    }

    for ($loop = $minPeriod; $loop <= $maxPeriod; $loop++)
    {
      for ($loop2 = $minPeriod; $loop2 <= $maxPeriod; $loop2++)
      {
        # do the test
        $s1 = "PLUGIN=TEST,NAME=$name,TAG=$maType-$input1-$loop-$input2-$loop2,SYMBOL=$exchange:$symbol";
        $s2 = "ENTER_LONG_NAME=$closeName,ENTER_LONG_OP=GT,ENTER_LONG_NAME2=buy$loop";
        $s3 = "EXIT_LONG_NAME=$closeName,EXIT_LONG_OP=LT,EXIT_LONG_NAME2=sell$loop2";
        $s4 = "DATE_NAME=$dateName,BUY_NAME=$buyField,SELL_NAME=$sellField,CLOSE_NAME=$closeName";
        $s5 = "DELAY=1,EQUITY=$equity,VOLUME=$volume";
        $command = "$s1,$s2,$s3,$s4,$s5";
        print STDOUT $command;
        $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }

        print STDERR "$name: $input1 $input2 $loop $loop2";
      }
    }
  }
}
