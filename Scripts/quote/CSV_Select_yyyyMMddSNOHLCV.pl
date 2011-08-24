# import quotes from a CSV file.
# format is yyyyMMdd,symbol,name,open,high,low,close,volume

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
            FORMAT=DATE,SYMBOL,NAME,OPEN,HIGH,LOW,CLOSE,VOLUME;
            DATE_FORMAT=yyyyMMdd;
            DELIMITER=Comma;
            TYPE=Stock";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the chart panel
$command = "COMMAND=CHART_PANEL_REFRESH;
            STEP=cp_refresh";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
