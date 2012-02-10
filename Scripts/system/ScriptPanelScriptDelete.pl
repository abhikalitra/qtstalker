# selects and deletes script(s) in the database

$scriptPanelCommand = 'scriptPanelCommand';
$scriptDBCommand = 'scriptDBCommand';
$confirmDialogCommand = 'confirmDialogCommand';
$confirmDialogText = "Confirm delete for the following:\n\n";

###################################################################
$|++;  # flush buffers
###################################################################

###################################################################
#  get script panel selected items
###################################################################

print STDOUT "NEW(SCRIPT_PANEL, $scriptPanelCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($scriptPanelCommand.METHOD, SELECT)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($scriptPanelCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  confirm delete dialog
###################################################################

# get the list of items selected
print STDOUT "GET($scriptPanelCommand.LIST)";
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
#  delete selected scripts from the database
###################################################################

print STDOUT "NEW(SCRIPT_DATABASE, $scriptDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($scriptDBCommand.METHOD, REMOVE,
                  $scriptDBCommand.LIST, $scriptPanelCommand.LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($scriptDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
