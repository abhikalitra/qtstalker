# selects and deletes script(s) in the database

$scriptDBCommand = 'scriptDBCommand';
$selectDialogCommand = 'selectDialogCommand';
$selectDialogTitle = 'Scripts';
$confirmDialogCommand = 'confirmDialogCommand';
$confirmDialogText = 'Confirm delete for the following:';

###################################################################
$|++;  # flush buffers
###################################################################

###################################################################
#  get list of scripts
###################################################################

print STDOUT "NEW(SCRIPT_DATABASE, $scriptDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($scriptDBCommand.METHOD, LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($scriptDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  SCRIPT SELECTION DIALOG
###################################################################

print STDOUT "NEW(DIALOG_SELECT, $selectDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($selectDialogCommand.LIST, $scriptDBCommand.LIST,
                  $selectDialogCommand.TITLE, $selectDialogTitle)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($selectDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  confirm delete dialog
###################################################################

# get the list of items selected
print STDOUT "GET($selectDialogCommand.LIST)";
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

print STDOUT "SET($scriptDBCommand.METHOD, REMOVE,
                  $scriptDBCommand.LIST, $selectDialogCommand.LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($scriptDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
