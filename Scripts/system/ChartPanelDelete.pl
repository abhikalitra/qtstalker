# delete symbols from the database

$|=1;

$command = "PLUGIN=SYMBOL_DIALOG,FLAG=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the symbols string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SYMBOL_DIALOG_SYMBOLS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

$command = "PLUGIN=QUOTE_DATABASE_DELETE,NAME=$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

$command = "PLUGIN=CHART_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
