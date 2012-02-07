# adds new script to script database

$scriptDBCommand = 'scriptDBCommand';
$scriptDialogCommand = 'scriptDialogCommand';

###################################################################
$|++;  # flush buffers
###################################################################

###################################################################
#  show script dialog
###################################################################

print STDOUT "NEW(DIALOG_SCRIPT, $scriptDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($scriptDialogCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  save script to database
###################################################################

print STDOUT "NEW(SCRIPT_DATABASE, $scriptDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($scriptDBCommand.METHOD, SAVE,
                  $scriptDBCommand.NAME, $scriptDialogCommand.NAME,
                  $scriptDBCommand.FILE, $scriptDialogCommand.FILE,
                  $scriptDBCommand.COMMAND, $scriptDialogCommand.COMMAND,
                  $scriptDBCommand.STARTUP, $scriptDialogCommand.STARTUP,
                  $scriptDBCommand.COMMENT, $scriptDialogCommand.COMMENT,
                  $scriptDBCommand.INTERVAL, $scriptDialogCommand.INTERVAL)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($scriptDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
