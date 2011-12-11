# QtStalker YahooHistoryDownload script

# yahoo dialog settings
$yahooDialogName = 'yahooSettings';
$yahooDialogTitle = 'Yahoo Download';
$yahooDialogTab = '0';

# date start setting
$dateStartKey = 'dateStartKey';
$dateStartLabel = 'Date Start';
$dateStartValue = '2010-12-11 00:00:00';

# date end setting
$dateEndKey = 'dateEndKey';
$dateEndLabel = 'Date End';
$dateEndValue = '2011-12-11 23:59:59';

# symbol file setting
$symbolFileKey = 'symbolFileKey';
$symbolFileLabel = 'Symbol File';
$symbolFileValue = '/home';

# csv file setting
$csvFileKey = 'csvFileKey';
$csvFileLabel = 'CSV File';
$csvFileValue = '/tmp/yahoo.csv';

# adjusted setting
$adjustedKey = 'adjustedKey';
$adjustedLabel = 'Adjusted';
$adjustedValue = 'true';

# CSV dialog settings
$csvDialogName = 'csvSettings';
$csvDialogTitle = 'Yahoo CSV Quote Import';
$csvDialogTab = '0';

# csv format setting
$csvFormatKey = 'csvFormatKey';
$csvFormatLabel = 'CSV Format';
$csvFormatValue = 'EXCHANGE;SYMBOL;NAME;DATE;OPEN;HIGH;LOW;CLOSE;VOLUME';

# csv date format setting
$csvDateFormatKey = 'csvDateFormatKey';
$csvDateFormatLabel = 'Date Format';
$csvDateFormatValue = 'yyyy-MM-dd';

# csv delimiter setting
$csvDelimiterKey = 'csvDelimiterKey';
$csvDelimiterLabel = 'Delimiter';
$csvDelimiterValue = 'Semicolon';
$csvDelimiterList = 'Comma;Semicolon';

# csv quote type setting
$csvQuoteTypeKey = 'csvQuoteTypeKey';
$csvQuoteTypeLabel = 'Quote Type';
$csvQuoteTypeValue = 'Stock';
$csvQuoteTypeList = 'Futures;Stock;Bond;Option;Mutual Fund;ETF';

# csv filename as symbol setting
$csvFilenameAsSymbolKey = 'csvFilenameAsSymbolKey';
$csvFilenameAsSymbolLabel = 'Use Filename As Symbol';
$csvFilenameAsSymbolValue = 'false';

# csv exchange setting
$csvExchangeKey = 'csvExchangeKey';
$csvExchangeLabel = 'Exchange Overide';
$csvExchangeValue = '';

################################################################################

# all perl scripts must turn on buffer flush flag otherwise script will hang
$|=1;

########################################################################################
############################# create the Yahoo dialog ####################################
########################################################################################

$command = "DIALOG($yahooDialogName, $yahooDialogTitle)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create date start setting
$command = "DATA_DATE_TIME($dateStartKey, $dateStartValue)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add setting to dialog
$command = "DIALOG_SET($yahooDialogName, $dateStartKey, $dateStartLabel, $yahooDialogTab)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create date end setting
$command = "DATA_DATE_TIME($dateEndKey, $dateEndValue)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add setting to dialog
$command = "DIALOG_SET($yahooDialogName, $dateEndKey, $dateEndLabel, $yahooDialogTab)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create symbol file setting
$command = "DATA_FILE($symbolFileKey, $symbolFileValue)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add setting to dialog
$command = "DIALOG_SET($yahooDialogName, $symbolFileKey, $symbolFileLabel, $yahooDialogTab)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create csv file setting
$command = "DATA_FILE($csvFileKey, $csvFileValue)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add setting to dialog
$command = "DIALOG_SET($yahooDialogName, $csvFileKey, $csvFileLabel, $yahooDialogTab)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create adjusted setting
$command = "DATA_BOOL($adjustedKey, $adjustedValue)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add setting to dialog
$command = "DIALOG_SET($yahooDialogName, $adjustedKey, $adjustedLabel, $yahooDialogTab)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# show the yahoo dialog
$command = "DIALOG_RUN($yahooDialogName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# we now pass the settings modified by the yahoo dialog to the yahoo history command
# download the quotes
$command = "YAHOO_HISTORY($dateStartKey, $dateEndKey, $adjustedKey, $csvFileKey, $symbolFileKey)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

########################################################################################
############################# create the CSV dialog ####################################
########################################################################################

$command = "DIALOG($csvDialogName, $csvDialogTitle)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create csv format setting
$command = "DATA_STRING($csvFormatKey, $csvFormatValue)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add csv format setting to dialog
$command = "DIALOG_SET($csvDialogName, $csvFormatKey, $csvFormatLabel, $csvDialogTab)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create csv date format setting
$command = "DATA_STRING($csvDateFormatKey, $csvDateFormatValue)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add csv date format setting to dialog
$command = "DIALOG_SET($csvDialogName, $csvDateFormatKey, $csvDateFormatLabel, $csvDialogTab)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create csv delimiter setting
$command = "DATA_LIST($csvDelimiterKey, $csvDelimiterValue, $csvDelimiterList)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add csv date format setting to dialog
$command = "DIALOG_SET($csvDialogName, $csvDelimiterKey, $csvDelimiterLabel, $csvDialogTab)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create csv quote type setting
$command = "DATA_LIST($csvQuoteTypeKey, $csvQuoteTypeValue, $csvQuoteTypeList)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add csv quote type setting to dialog
$command = "DIALOG_SET($csvDialogName, $csvQuoteTypeKey, $csvQuoteTypeLabel, $csvDialogTab)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create csv filename as symbol setting
$command = "DATA_BOOL($csvFilenameAsSymbolKey, $csvFilenameAsSymbolValue)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add csv filename as symbol setting to dialog
$command = "DIALOG_SET($csvDialogName, $csvFilenameAsSymbolKey, $csvFilenameAsSymbolLabel, $csvDialogTab)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create csv exchange setting
$command = "DATA_STRING($csvExchangeKey, $csvExchangeValue)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# add csv exchange setting to dialog
$command = "DIALOG_SET($csvDialogName, $csvExchangeKey, $csvExchangeLabel, $csvDialogTab)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# show the CSV dialog
$command = "DIALOG_RUN($csvDialogName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# we pass the settings modified by the dialog to the CSV command
# import the CSV file into the quote database
$command = "CSV($csvFileKey, $csvFormatKey, $csvDateFormatKey, $csvDelimiterKey, $csvQuoteTypeKey, $csvFilenameAsSymbolKey, $csvExchangeKey)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the chart panel
$command = "CHART_PANEL_REFRESH()";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
