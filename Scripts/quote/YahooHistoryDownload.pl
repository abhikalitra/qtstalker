# QtStalker YahooHistoryDownload script

$yahooCommand = 'YAHOO_HISTORY';
$yahooCommandName = 'downloader';
$dateStartSetting = 'DATE_START';
$dateStartValue = '2011-01-01 00:00:00';
$dateEndSetting = 'DATE_END';
$dateEndValue = '2012-01-01 23:59:59';
$symbolFileSetting = 'SYMBOL_FILE';
$symbolFileValue = '~/.qtstalker/yahoo/symbols';
$csvFileSetting = 'CSV_FILE';
$csvFileValue = '~/.qtstalker/yahoo/yahoo.csv';
$csvCommand = 'CSV';
$csvCommandName = 'csv';
$csvDelimiterSetting = 'DELIMITER';
$csvDelimiterValue = 'Semicolon';

################################################################################
# all perl scripts must turn on buffer flush flag otherwise script will hang
$|=1;
########################################################################################

########################################################################################
############################# create the Yahoo dialog ####################################
########################################################################################

# create the yahoo_history object
$command = "NEW($yahooCommand, $yahooCommandName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# set the yahoo_history settings
$command = "SET($yahooCommandName, $dateStartSetting, $dateStartValue,
                $yahooCommandName, $csvFileSetting, $csvFileValue,
                $yahooCommandName, $symbolFileSetting, $symbolFileValue)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# show dialog
$command = "DIALOG($yahooCommandName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# download the quotes
$command = "RUN($yahooCommandName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

########################################################################################
############################# create the CSV dialog ####################################
########################################################################################

# create the CSV object
$command = "NEW($csvCommand, $csvCommandName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the csvFileSetting value from the yahoo_history object
$command = "GET($yahooCommandName, $csvFileSetting)";
print STDOUT $command;
$csvFileValue = <STDIN>; chomp($csvFileValue); if ($csvFileValue eq "ERROR") {print STDERR $command; exit; }

# set the CSV settings
$command = "SET($csvCommandName, $csvFileSetting, $csvFileValue,
                $csvCommandName, $csvDelimiterSetting, $csvDelimiterValue)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# show CSV dialog
$command = "DIALOG($csvCommandName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# import the CSV file into the quote database
$command = "RUN($csvCommandName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

########################################################################################
############################# refresh chart panel ####################################
########################################################################################

# create the chart panel refresh object
$command = "NEW(CHART_PANEL_REFRESH, refresh)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the chart panel
$command = "RUN(refresh)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
