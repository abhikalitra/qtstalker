# QtStalker YahooHistoryDownload script

$yahooCommand = 'YAHOO_HISTORY';
$yahooCommandName = 'downloader';
$yahooDialogSettingsKey = 'yahoo_history_download_yahoo_dialog_settings';
$csvCommand = 'CSV';
$csvCommandName = 'csv';
$csvDialogSettingsKey = 'yahoo_history_download_csv_dialog_settings';
$refreshCommand = 'refreshCommand';

################################################################################
# all perl scripts must turn on buffer flush flag otherwise script will hang
$|=1;
########################################################################################

########################################################################################
# create the Yahoo dialog
########################################################################################

# create the yahoo_history object
print STDOUT "NEW($yahooCommand, $yahooCommandName)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# load saved yahoo dialog settings
print STDOUT "LOAD($yahooCommandName, $yahooDialogSettingsKey)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# show dialog
print STDOUT "DIALOG($yahooCommandName)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# save yahoo dialog settings
print STDOUT "SAVE($yahooCommandName, $yahooDialogSettingsKey)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($yahooCommandName)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

########################################################################################
# create the CSV dialog
########################################################################################

# create the CSV object
print STDOUT "NEW($csvCommand, $csvCommandName)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# load saved csv dialog settings
print STDOUT "LOAD($csvCommandName, $csvDialogSettingsKey)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the csv dialog csv file location
print STDOUT "SET($csvCommandName.CSV_FILE, $yahooCommandName.CSV_FILE)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# show CSV dialog
print STDOUT "DIALOG($csvCommandName)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# save yahoo dialog settings
print STDOUT "SAVE($csvCommandName, $csvDialogSettingsKey)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# import the CSV file into the quote database
print STDOUT "RUN($csvCommandName)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

########################################################################################
# refresh chart panel
########################################################################################

# create the chart panel refresh object
print STDOUT "NEW(CHART_PANEL, $refreshCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($refreshCommand.METHOD, REFRESH)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# refresh the chart panel
print STDOUT "RUN($refreshCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
