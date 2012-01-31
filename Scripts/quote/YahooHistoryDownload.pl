# QtStalker YahooHistoryDownload script

$yahooCommand = 'YAHOO_HISTORY';
$yahooCommandName = 'downloader';
$yahooDialogSettingsKey = 'yahoo_history_download_yahoo_dialog_settings';
$csvCommand = 'CSV';
$csvCommandName = 'csv';
$csvDialogSettingsKey = 'yahoo_history_download_csv_dialog_settings';

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
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# load saved yahoo dialog settings
$command = "LOAD($yahooCommandName, $yahooDialogSettingsKey)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# show dialog
$command = "DIALOG($yahooCommandName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# save yahoo dialog settings
$command = "SAVE($yahooCommandName, $yahooDialogSettingsKey)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

$command = "RUN($yahooCommandName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

########################################################################################
############################# create the CSV dialog ####################################
########################################################################################

# create the CSV object
$command = "NEW($csvCommand, $csvCommandName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# load saved csv dialog settings
$command = "LOAD($csvCommandName, $csvDialogSettingsKey)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# set the csv dialog csv file location
$command = "SET($csvCommandName.CSV_FILE, $yahooCommandName.CSV_FILE)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# show CSV dialog
$command = "DIALOG($csvCommandName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# save yahoo dialog settings
$command = "SAVE($csvCommandName, $csvDialogSettingsKey)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# import the CSV file into the quote database
$command = "RUN($csvCommandName)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

########################################################################################
############################# refresh chart panel ####################################
########################################################################################

# create the chart panel refresh object
$command = "NEW(CHART_PANEL_REFRESH, refresh)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# refresh the chart panel
$command = "RUN(refresh)";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
