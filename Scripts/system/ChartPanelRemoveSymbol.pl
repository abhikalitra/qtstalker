# removes selected symbols from the chart panel from the quote database

$chartPanelCommand = 'chartPanelCommand';
$confirmDialogCommand = 'confirmDialog';
$confirmDialogText = "Confirm deleting selected symbols from the database\n\n";
$symbolDBCommand = 'symbolDBCommand';

###################################################################
$|++;  # flush buffers
###################################################################

###################################################################
#  GET CHART PANEL SELECTED SYMBOLS
###################################################################

print STDOUT "NEW(CHART_PANEL, $chartPanelCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($chartPanelCommand.METHOD, SELECT)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($chartPanelCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  CONFIRM DIALOG
###################################################################

# get the list of items selected
print STDOUT "GET($chartPanelCommand.LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# construct the message for the dialog
$rc =~ s/;/\n/;  # replace semicolon delimiter with newline
$confirmDialogText = $confirmDialogText . $rc;

print STDOUT "NEW(DIALOG_CONFIRM, $confirmDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($confirmDialogCommand.TEXT, $confirmDialogText)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($confirmDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  REMOVE SYMBOLS
###################################################################

print STDOUT "NEW(SYMBOL_DATABASE, $symbolDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($symbolDBCommand.METHOD, REMOVE,
                  $symbolDBCommand.LIST, $chartPanelCommand.LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($symbolDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  REFRESH CHART PANEL
###################################################################

print STDOUT "SET($chartPanelCommand.METHOD, REFRESH)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($chartPanelCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
