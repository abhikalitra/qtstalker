# display symbols in the chart panel from the symbol dialog

$|=1;

# display the symbol search dialog
$command = "PLUGIN=SYMBOL_DIALOG,FLAG=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the exchange string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SYMBOL_DIALOG_EXCHANGE";
print STDOUT $command;
$exchange = <STDIN>; chomp($exchange); if ($exchange eq "ERROR") {print STDERR $command; exit; }

# get the symbol string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SYMBOL_DIALOG_SYMBOL";
print STDOUT $command;
$symbol = <STDIN>; chomp($symbol); if ($symbol eq "ERROR") {print STDERR $command; exit; }

# get symbols selected from the chart panel
$command = "PLUGIN=CHART_PANEL_SEARCH,EXCHANGE=$exchange,SYMBOL=$symbol";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the chart panel
$command = "PLUGIN=CHART_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
