# adds a new indicator to app

###################################################################
$|++;  # flush buffers
###################################################################

###################################################################
###################  SCRIPT RUN DIALOG  ################################
###################################################################

print STDOUT "NEW(DIALOG_SCRIPT, srd)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN(srd)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  SAVE INDICATOR  ################################
###################################################################

print STDOUT "NEW(INDICATOR, indicator)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET(indicator.METHOD, ADD,
                  indicator.FILE, srd.FILE,
                  indicator.COMMAND, srd.COMMAND)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN(indicator)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
###################  RUN INDICATOR  ################################
###################################################################

print STDOUT "NEW(SCRIPT, script)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET(script.FILE, srd.FILE,
                  script.COMMAND, srd.COMMAND)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN(script)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
