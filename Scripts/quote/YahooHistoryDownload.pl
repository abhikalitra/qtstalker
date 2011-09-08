# download yahoo historical quotes

$symbolFile = '/tmp/yahoo_symbols';
$csvFile = '/tmp/yahoo_csv';

################################################################################

$|=1;

# show the dialog
$command = "COMMAND=YAHOO_HISTORY;
            DATE_START=;
            DATE_END=;
            SYMBOL_FILE=$symbolFile;
            CSV_FILE=$csvFile;
            ADJUSTED=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# import the CSV file into the quote database
$command = "COMMAND=CSV;
            CSV_FILE=$csvFile;
            FORMAT=EXCHANGE,SYMBOL,NAME,DATE,OPEN,HIGH,LOW,CLOSE,VOLUME;
            DATE_FORMAT=yyyy-MM-dd;
            DELIMITER=Semicolon;
            TYPE=Stock;
            FILENAME_AS_SYMBOL=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the chart panel
$command = "COMMAND=CHART_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
