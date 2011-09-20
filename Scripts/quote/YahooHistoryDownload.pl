
$dateStartKey = 'Date Start';
$dateEndKey = 'Date End';
$symbolFileKey = 'Symbol File';
$csvFileKey = 'CSV File';
$csvFile = '/tmp/yahoo.csv';
$adjustedKey = 'Adjusted';

$formatKey = 'Format';
$format = 'EXCHANGE,SYMBOL,NAME,DATE,OPEN,HIGH,LOW,CLOSE,VOLUME';

$dateFormatKey = 'Date Format';
$dateFormat = 'yyyy-MM-dd';

$delimiterKey = 'Delimiter';
$delimiter = 'Semicolon';
$delimiterList = 'Comma,Semicolon';

$typeKey = 'Quote Type';
$type = 'Stock';
$typeList = 'Futures,Stock,Bond,Option,Mutual Fund,ETF';

$filenameAsSymbolKey = 'Use Filename As Symbol';
$filenameAsSymbol = 'false';

################################################################################

$|=1;

# create start date setting
$command = "COMMAND=SETTING; KEY=$dateStartKey; TYPE=DATETIME";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create end date setting
$command = "COMMAND=SETTING; KEY=$dateEndKey; TYPE=DATETIME";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create symbol file setting
$command = "COMMAND=SETTING; KEY=$symbolFileKey; TYPE=FILE";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create csv file setting
$command = "COMMAND=SETTING; KEY=$csvFileKey; VALUE=$csvFile; TYPE=FILE";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create adjusted setting
$command = "COMMAND=SETTING; KEY=$adjustedKey; VALUE=true; TYPE=BOOL";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# show the Yahoo dialog
$command = "COMMAND=DIALOG;
            TITLE=Yahoo Quotes Download;
            SETTING_0=$dateStartKey;
            SETTING_1=$dateEndKey;
            SETTING_2=$symbolFileKey;
            SETTING_3=$csvFileKey;
            SETTING_4=$adjustedKey";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# download the quotes
$command = "COMMAND=YAHOO_HISTORY;
            DATE_START=$dateStartKey;
            DATE_END=$dateEndKey;
            SYMBOL_FILE=$symbolFileKey;
            CSV_FILE=$csvFileKey;
            ADJUSTED=$adjustedKey";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create CSV dialog settings
# create FORMAT setting
$command = "COMMAND=SETTING; KEY=$formatKey; VALUE=$format; TYPE=STRING";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create DATE_FORMAT setting
$command = "COMMAND=SETTING; KEY=$dateFormatKey; VALUE=$dateFormat; TYPE=STRING";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create DELIMITER setting
$command = "COMMAND=SETTING; KEY=$delimiterKey; VALUE=$delimiter; LIST=$delimiterList; TYPE=LIST";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create TYPE setting
$command = "COMMAND=SETTING; KEY=$typeKey; VALUE=$type; LIST=$typeList; TYPE=LIST";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create FILENAME_AS_SYMBOL setting
$command = "COMMAND=SETTING; KEY=$filenameAsSymbolKey; VALUE=$filenameAsSymbol; TYPE=BOOL";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# show the CSV dialog
$command = "COMMAND=DIALOG;
            TITLE=CSV Settings;
            SETTING_0=$csvFileKey;
            SETTING_1=$formatKey;
            SETTING_2=$dateFormatKey;
            SETTING_3=$delimiterKey;
            SETTING_4=$typeKey;
            SETTING_5=$filenameAsSymbolKey";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# import the CSV file into the quote database
$command = "COMMAND=CSV;
            CSV_FILE=$csvFileKey;
            FORMAT=$formatKey;
            DATE_FORMAT=$dateFormatKey;
            DELIMITER=$delimiterKey;
            TYPE=$typeKey;
            FILENAME_AS_SYMBOL=$filenameAsSymbolKey";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the chart panel
$command = "COMMAND=CHART_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
