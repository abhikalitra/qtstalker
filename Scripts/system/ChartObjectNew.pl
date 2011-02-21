# new chart object

$|=1;

# get indicator chart object will be attached to
$command = "PLUGIN=SETTINGS_LOAD,KEY=chart_object_new_indicator";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the indicator string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SETTINGS_LOAD_DATA";
print STDOUT $command;
$indicator = <STDIN>; chomp($indicator); if ($indicator eq "ERROR") {print STDERR $command; exit; }

# get the chart object type to create
$command = "PLUGIN=SETTINGS_LOAD,KEY=chart_object_new";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the type string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SETTINGS_LOAD_DATA";
print STDOUT $command;
$type = <STDIN>; chomp($type); if ($type eq "ERROR") {print STDERR $command; exit; }

# get the current symbol and exchange
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

# get current chart object id's from database
$command = "PLUGIN=CHART_OBJECT_DATABASE_LIST,INDICATOR=$indicator,EXCHANGE=$exchange,SYMBOL=$symbol";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the chart object id's string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=CHART_OBJECT_DATABASE_LIST_NAME";
print STDOUT $command;
$list = <STDIN>; chomp($list); if ($list eq "ERROR") {print STDERR $command; exit; }

# display new dialog
$command = "PLUGIN=NEW_DIALOG,TITLE=chart object name,ITEMS=$list";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the new chart object name string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=NEW_DIALOG_NAME";
print STDOUT $command;
$name = <STDIN>; chomp($name); if ($name eq "ERROR") {print STDERR $command; exit; }

# notify app to create new chart object
$command = "PLUGIN=CHART_OBJECT_NEW,INDICATOR=$indicator,NAME=$name,TYPE=$type";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
