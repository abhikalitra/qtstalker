# selects and edits a script in the script database

$scriptPanelCommand = 'scriptPanelCommand';
$scriptDBCommand = 'scriptDBCommand';
$scriptDialogCommand = 'scriptDialogCommand';

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
#  load selected script
###################################################################

# get the list of items selected
print STDOUT "GET($scriptPanelCommand.LIST)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
my @names = split(';', $rc);

print STDOUT "NEW(SCRIPT_DATABASE, $scriptDBCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "SET($scriptDBCommand.METHOD, LOAD,
                  $scriptDBCommand.NAME, $names[0])";
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
