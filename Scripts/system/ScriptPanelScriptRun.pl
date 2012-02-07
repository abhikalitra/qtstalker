# displays the run script dialog and runs selected script

$dialogScriptCommand = 'dialogScriptCommand';
$scriptDBCommand = 'scriptDBCommand';
$scriptCommand = 'scriptCommand';

###################################################################
$|++;  # flush buffers
###################################################################

###################################################################
#  SHOW RUN SCRIPT DIALOG
###################################################################

print STDOUT "NEW(DIALOG_SCRIPT, $dialogScriptCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($dialogScriptCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

###################################################################
#  save script if startup or interval parm has changed
###################################################################

print STDOUT "GET($dialogScriptCommand.STARTUP)";
$startup = <STDIN>; chomp($startup); if ($startup eq "ERROR") { exit; }

print STDOUT "GET($dialogScriptCommand.RUN_INTERVAL)";
$interval = <STDIN>; chomp($interval); if ($interval eq "ERROR") { exit; }

if ($startup eq 'true' or $interval gt 0)
{
  print STDOUT "NEW(SCRIPT_DATABASE, $scriptDBCommand)";
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

  print STDOUT "SET($scriptDBCommand.METHOD, SAVE,
                    $scriptDBCommand.FILE, $dialogScriptCommand.FILE,
                    $scriptDBCommand.COMMAND, $dialogScriptCommand.COMMAND,
                    $scriptDBCommand.STARTUP, $dialogScriptCommand.STARTUP,
                    $scriptDBCommand.RUN_INTERVAL, $dialogScriptCommand.RUN_INTERVAL)";
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

  print STDOUT "RUN($scriptDBCommand)";
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
}

###################################################################
#  run script
###################################################################

print STDOUT "NEW(SCRIPT, $scriptCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

  print STDOUT "SET($scriptCommand.FILE, $dialogScriptCommand.FILE,
                    $scriptCommand.COMMAND, $dialogScriptCommand.COMMAND)";
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

print STDOUT "RUN($scriptCommand)";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
