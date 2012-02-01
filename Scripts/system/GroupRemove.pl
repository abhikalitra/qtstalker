# deletes groups from the database

$groupDBCommand = 'groupDBCommand';
$listDialogCommand = 'listDialog';
$listDialogTitle = 'Groups';
$groupPanelRefreshCommand = 'groupPanelRefreshCommand';

###################################################################
$|++;  # flush buffers
###################################################################

###################################################################
###################  GET GROUPS  #######################
###################################################################

print STDOUT "NEW(GROUP_DATABASE, $groupDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($groupDBCommand.METHOD, LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($groupDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  GROUP SELECTION DIALOG  ##################
###################################################################

print STDOUT "NEW(DIALOG_SELECT, $listDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($listDialogCommand.LIST, $groupDBCommand.LIST,
                  $listDialogCommand.TITLE, $listDialogTitle)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($listDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  REMOVE GROUPS  ##################
###################################################################

print STDOUT "SET($groupDBCommand.METHOD, REMOVE,
                  $groupDBCommand.LIST, $listDialogCommand.LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($groupDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  REFRESH GROUP PANEL
###################################################################

print STDOUT "NEW(GROUP_PANEL_REFRESH, $groupPanelRefreshCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($groupPanelRefreshCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
