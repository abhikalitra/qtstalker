# download yahoo historical quotes

$|=1;

################################################################################

#show the dialog
$command = "COMMAND=YAHOO_HISTORY;
            STEP=yahoo;
            DATE_START=;
            DATE_END=;
            SYMBOL_FILE=/media/data/mystuff/prog/qtstalker/yahoo_symbols;
            CSV_FILE=/media/data/mystuff/prog/qtstalker/yahoo_csv;
            ADJUSTED=true";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# import the CSV file into the quote database
$command = "COMMAND=CSV;
            STEP=csv;
            CSV_FILE=yahoo:CSV_FILE;
            FORMAT=SYMBOL,NAME,DATE,OPEN,HIGH,LOW,CLOSE,VOLUME;
            DATE_FORMAT=yyyy-MM-dd;
            DELIMITER=Semicolon;
            TYPE=Stock";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the chart panel
$command = "COMMAND=CHART_PANEL_REFRESH;
            STEP=cp_refresh";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
