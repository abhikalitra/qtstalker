########################################################################################################
# adjusts quotes in the database for a stock split
# dialog propmts for a symbol, date and split ratio e.g. 2:1, 3:1, 1:10 etc.
# 
########################################################################################################

$dateName = 'Date';
$openName = 'Open';
$highName = 'High';
$lowName = 'Low';
$closeName = 'Close';
$closeName = 'Volume';

###############################################################################################################

$|++;

# display input dialog
$s1 = "PLUGIN=INPUT_DIALOG,INPUT1_LABEL=Symbol,INPUT1_TYPE=SYMBOL";
$s2 = "INPUT2_LABEL=Split Date,INPUT2_TYPE=DATE";
$s3 = "INPUT3_LABEL=Split,INPUT3_TYPE=TEXT,INPUT3_VALUE=2:1";
$command = "$s1,$s2,$s3";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the symbol string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT1_DATA";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

my @symbols = split(';', $rc);
my @parts = split(':', $symbols[0]);
$exchange = $parts[0];
$symbol = $parts[1];

# get the date string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT2_DATA";
print STDOUT $command;
$date = <STDIN>; chomp($date); if ($date eq "ERROR") {print STDERR $command; exit; }

# get the split string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT3_DATA";
print STDOUT $command;
$split = <STDIN>; chomp($split); if ($split eq "ERROR") {print STDERR $command; exit; }

# do the split
$command = "PLUGIN=STOCK_SPLIT,EXCHANGE=$exchange,SYMBOL=$symbol,DATE=$date,SPLIT=$split";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the chart panel
$command = "PLUGIN=CONTROL_PANEL,METHOD=CHART_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
