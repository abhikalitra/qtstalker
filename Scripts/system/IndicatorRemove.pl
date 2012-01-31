# deletes an indicator

$indicatorDBCommand = 'idb';

$listDialogCommand = 'listDialog';
$listDialogTitle = 'Remove Indicators';

###################################################################
$|++;  # flush buffers
###################################################################

###################################################################
###################  GET ACTIVE INDICATORS  #######################
###################################################################

print STDOUT "NEW(INDICATOR, $indicatorDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($indicatorDBCommand.METHOD, LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($indicatorDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  INDICATOR SELECTION DIALOG  ##################
###################################################################

print STDOUT "NEW(DIALOG_SELECT, $listDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($listDialogCommand.LIST, $indicatorDBCommand.LIST,
                  $listDialogCommand.TITLE, $listDialogTitle)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($listDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  REMOVE INDICATORS  ##################
###################################################################

print STDOUT "SET($indicatorDBCommand.METHOD, REMOVE,
                  $indicatorDBCommand.LIST, $listDialogCommand.LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($indicatorDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  REMOVE PLOTS  ##################
###################################################################

print STDOUT "NEW(CHART_REMOVE, crc)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET(crc.LIST, $listDialogCommand.LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN(crc)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
