# deletes chart objects from selected indicators

$indicatorDBCommand = 'idb';
$listDialogCommand = 'listDialog';
$listDialogTitle = 'Indicators';
$confirmDialogCommand = 'confirmDialog';
$confirmDialogText = "Confirm removing all chart objects from following indicators\n\n";
$chartObjectRemoveCommand = 'chartObjectRemove';

###################################################################
$|++;  # flush buffers
###################################################################

###################################################################
#  GET ACTIVE INDICATORS
###################################################################

print STDOUT "NEW(INDICATOR, $indicatorDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($indicatorDBCommand.METHOD, LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($indicatorDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  INDICATOR SELECTION DIALOG
###################################################################

print STDOUT "NEW(DIALOG_SELECT, $listDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($listDialogCommand.LIST, $indicatorDBCommand.LIST,
                  $listDialogCommand.TITLE, $listDialogTitle)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($listDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  CONFIRM DIALOG
###################################################################

# get the list of items selected
print STDOUT "GET($listDialogCommand.LIST)";
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
#  REMOVE CHART OBJECTS FROM INDICATORS
###################################################################

print STDOUT "NEW(CHART_OBJECT_REMOVE, $chartObjectRemoveCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($chartObjectRemoveCommand.LIST, $listDialogCommand.LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($chartObjectRemoveCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

