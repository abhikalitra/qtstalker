# edit chart object

$|=1;

# get chart object name
$command = "PLUGIN=SETTINGS_LOAD,KEY=chart_object_edit_id";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the chart object name string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=SETTINGS_LOAD_DATA";
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
