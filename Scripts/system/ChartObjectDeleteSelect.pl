# deletes 1 or more chart objects from a selection dialog

$|=1;

# get indicator to select from
$command = "PLUGIN=SETTINGS_LOAD,KEY=chart_object_delete_data";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get indicator string
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
$command = "PLUGIN=CHART_OBJECT_DATABASE,METHOD=IDS,INDICATOR=$indicator,EXCHANGE=$exchange,SYMBOL=$symbol";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the chart object ids
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=CHART_OBJECT_DATABASE_IDS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# display select dialog
$command = "PLUGIN=SELECT_DIALOG,MODE=0,TITLE=Chart Objects,ITEMS=$rc";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get selected chart object ids
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SELECT_DIALOG_SELECTED";
print STDOUT $command;
$list = <STDIN>; chomp($list); if ($list eq "ERROR") {print STDERR $command; exit; }

# confirm delete
$command = "PLUGIN=CONFIRM_DIALOG,MESSAGE=Confirm chart object delete";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# split list string into an array
my @names = split(',', $list);

# loop through each item in the array and delete it from the database
foreach $item (@names)
{
  # delete chart object from database
  $command = "PLUGIN=CHART_OBJECT_DATABASE,METHOD=DELETE,ID=$item";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }
}

# update the app
$command = "PLUGIN=CHART_OBJECT_DELETE,LIST=$list";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
