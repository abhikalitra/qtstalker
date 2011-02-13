# edit chart object from selection dialog

$|=1;

# get indicator to select from
$command = "PLUGIN=SETTINGS_LOAD,KEY=chart_object_edit_data";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the chart object name string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SETTINGS_LOAD_DATA";
print STDOUT $command;
$indicator = <STDIN>; chomp($indicator); if ($indicator eq "ERROR") {print STDERR $command; exit; }

# current symbol
$command = "PLUGIN=SYMBOL_CURRENT";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the exchange string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SYMBOL_CURRENT_EXCHANGE";
print STDOUT $command;
$exchange = <STDIN>; chomp($exchange); if ($exchange eq "ERROR") {print STDERR $command; exit; }

# get the symbol string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SYMBOL_CURRENT_SYMBOL";
print STDOUT $command;
$symbol = <STDIN>; chomp($symbol); if ($symbol eq "ERROR") {print STDERR $command; exit; }

# get current chart objects from database allocated to the indicator
$command = "PLUGIN=CHART_OBJECT_DATABASE_LIST,INDICATOR=$indicator,EXCHANGE=$exchange,SYMBOL=$symbol";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the chart object names
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=CHART_OBJECT_DATABASE_LIST_NAME";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# display select dialog
$command = "PLUGIN=SELECT_DIALOG,MODE=1,TITLE=Chart Objects,ITEMS=$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get selected chart object ids
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SELECT_DIALOG_SELECTED";
print STDOUT $command;
$name = <STDIN>; chomp($name); if ($name eq "ERROR") {print STDERR $command; exit; }

# get the chart object type from the database
$command = "PLUGIN=CHART_OBJECT_DATABASE_TYPE,ID=$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the type string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=CHART_OBJECT_DATABASE_TYPE_TYPE";
print STDOUT $command;
$type = <STDIN>; chomp($type); if ($type eq "ERROR") {print STDERR $command; exit; }

if ($type eq "Buy") { $command = "PLUGIN=CHART_OBJECT_BUY_DIALOG,ID=$name"; }
elsif ($type eq "HLine") { $command = "PLUGIN=CHART_OBJECT_HLINE_DIALOG,ID=$name"; }
elsif ($type eq "Retracement") { $command = "PLUGIN=CHART_OBJECT_RETRACEMENT_DIALOG,ID=$name"; }
elsif ($type eq "Sell") { $command = "PLUGIN=CHART_OBJECT_SELL_DIALOG,ID=$name"; }
elsif ($type eq "Text") { $command = "PLUGIN=CHART_OBJECT_TEXT_DIALOG,ID=$name"; }
elsif ($type eq "TLine") { $command = "PLUGIN=CHART_OBJECT_TLINE_DIALOG,ID=$name"; }
elsif ($type eq "VLine") { $command = "PLUGIN=CHART_OBJECT_VLINE_DIALOG,ID=$name"; }

# display chart object dialog
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update the app
$command = "PLUGIN=CHART_OBJECT_UPDATE,NAME=$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
