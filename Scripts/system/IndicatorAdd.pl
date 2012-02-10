# adds a new indicator to app

$dialogScriptCommand = 'dialogScriptCommand';
$indicatorCommand = 'indicatorCommand';
$chartPanelCommand = 'chartPanelCommand';

###################################################################
$|++;  # flush buffers
###################################################################

###################################################################
###################  SCRIPT RUN DIALOG  ################################
###################################################################

print STDOUT "NEW(DIALOG_SCRIPT, $dialogScriptCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($dialogScriptCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  SAVE INDICATOR  ################################
###################################################################

print STDOUT "NEW(INDICATOR, $indicatorCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($indicatorCommand.METHOD, SAVE,
                  $indicatorCommand.NAME, $dialogScriptCommand.NAME,
                  $indicatorCommand.FILE, $dialogScriptCommand.FILE,
                  $indicatorCommand.COMMAND, $dialogScriptCommand.COMMAND)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($indicatorCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  RUN INDICATOR  ################################
###################################################################

print STDOUT "NEW(CHART_PANEL, $chartPanelCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($chartPanelCommand.METHOD, RELOAD)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($chartPanelCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
