# import quotes from a CSV file with filename as symbol.
# format is yyyyMMdd,open,high,low,close,volume

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
            FORMAT=DATE,OPEN,HIGH,LOW,CLOSE,VOLUME;
            DATE_FORMAT=yyyyMMdd;
            DELIMITER=Comma;
            TYPE=Stock;
            FILENAME_AS_SYMBOL=true";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the chart panel
$command = "COMMAND=CHART_PANEL_REFRESH;
            STEP=cp_refresh";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
