# selects and edits a script in the script database

$scriptDBCommand = 'scriptDBCommand';
$selectDialogCommand = 'selectDialogCommand';
$selectDialogTitle = 'Scripts';
$scriptDialogCommand = 'scriptDialogCommand';

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
###################  SCRIPT SELECTION DIALOG  ##################
###################################################################

print STDOUT "NEW(DIALOG_SELECT, $selectDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($selectDialogCommand.LIST, $scriptDBCommand.LIST,
                  $selectDialogCommand.TITLE, $selectDialogTitle,
                  $selectDialogCommand.MODE, 1)"; # single selection only
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($selectDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  load selected script
###################################################################

print STDOUT "SET($scriptDBCommand.METHOD, LOAD,
                  $scriptDBCommand.NAME, $selectDialogCommand.LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($scriptDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  show script dialog
###################################################################

print STDOUT "NEW(DIALOG_SCRIPT, $scriptDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($scriptDialogCommand.FILE, $scriptDBCommand.FILE,
                  $scriptDialogCommand.COMMAND, $scriptDBCommand.COMMAND,
                  $scriptDialogCommand.NAME, $scriptDBCommand.NAME,
                  $scriptDialogCommand.INTERVAL, $scriptDBCommand.INTERVAL,
                  $scriptDialogCommand.COMMENT, $scriptDBCommand.COMMENT,
                  $scriptDialogCommand.STARTUP, $scriptDBCommand.STARTUP)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($scriptDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  save script to database
###################################################################

print STDOUT "SET($scriptDBCommand.METHOD, SAVE,
                  $scriptDBCommand.FILE, $scriptDialogCommand.FILE,
                  $scriptDBCommand.COMMAND, $scriptDialogCommand.COMMAND,
                  $scriptDBCommand.NAME, $scriptDialogCommand.NAME,
                  $scriptDBCommand.COMMENT, $scriptDialogCommand.COMMENT,
                  $scriptDBCommand.STARTUP, $scriptDialogCommand.STARTUP,
                  $scriptDBCommand.INTERVAL, $scriptDialogCommand.INTERVAL)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($scriptDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
