# puts selected symbols from the chart panel into selected group

$chartPanelCommand = 'chartPanelCommand';
$groupDBCommand = 'groupDBCommand';
$listDialogCommand = 'listDialog';
$listDialogTitle = 'Group';
$groupPanelRefreshCommand = 'groupPanelRefreshCommand';

###################################################################
$|++;  # flush buffers
###################################################################

###################################################################
#  GET CHART PANEL SELECTED SYMBOLS
###################################################################

print STDOUT "NEW(CHART_PANEL_SELECT, $chartPanelCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($chartPanelCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  GET GROUPS
###################################################################

print STDOUT "NEW(GROUP_DATABASE, $groupDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($groupDBCommand.METHOD, LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($groupDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  GROUP SELECTION DIALOG
###################################################################

print STDOUT "NEW(DIALOG_SELECT, $listDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($listDialogCommand.LIST, $groupDBCommand.LIST,
                  $listDialogCommand.TITLE, $listDialogTitle,
                  $listDialogCommand.MODE, 1)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($listDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  ADD SELECTED SYMBOLS TO GROUP
###################################################################

print STDOUT "GET($listDialogCommand.LIST)";
$name = <STDIN>; chomp($name); if ($name eq "ERROR") { exit; }

print STDOUT "SET($groupDBCommand.METHOD, ADD,
                  $groupDBCommand.NAME, $name,
                  $groupDBCommand.LIST, $chartPanelCommand.LIST)";
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
