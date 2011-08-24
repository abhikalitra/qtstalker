# import symbols that were previously exported by QtStalker Export function

$|=1;

################################################################################

# display the file selection dialog
$command = "COMMAND=FILE_DIALOG;
            STEP=fileDialog";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

$command = "COMMAND=CSV;
            STEP=csv;
            CSV_FILE=fileDialog:FILES;
            FORMAT=SYMBOL,NAME,DATE,OPEN,HIGH,LOW,CLOSE,VOLUME,OI;
            DATE_FORMAT=yyyy-MM-ddTHH:mm:ss;
            DELIMITER=Comma;
            TYPE=Stock";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the chart panel
$command = "COMMAND=CHART_PANEL_REFRESH;
            STEP=cp_refresh";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
