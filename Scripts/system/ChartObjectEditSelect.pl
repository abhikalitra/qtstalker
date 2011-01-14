# edit chart object

$|=1;

# get indicator to select from
$command = "SETTINGS,LOAD,chart_object_edit_data";
print STDOUT $command;
$indicator = <STDIN>; chomp($indicator); if ($indicator eq "ERROR") {print STDERR $command; exit; }

# get current chart objects from database
$command = "CHART_OBJECT_DATABASE,IDS,$indicator";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# display select dialog
$command = "SELECT_DIALOG,1,Chart Objects,$rc";
print STDOUT $command;
$name = <STDIN>; chomp($name); if ($name eq "ERROR") {print STDERR $command; exit; }

# get the chart object type from the database
$command = "CHART_OBJECT_DATABASE,TYPE,$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

if ($rc eq "0") { $command = "CHART_OBJECT_BUY_DIALOG,$name"; }
elsif ($rc eq "1") { $command = "CHART_OBJECT_HLINE_DIALOG,$name"; }
elsif ($rc eq "2") { $command = "CHART_OBJECT_RETRACEMENT_DIALOG,$name"; }
elsif ($rc eq "3") { $command = "CHART_OBJECT_SELL_DIALOG,$name"; }
elsif ($rc eq "4") { $command = "CHART_OBJECT_TEXT_DIALOG,$name"; }
elsif ($rc eq "5") { $command = "CHART_OBJECT_TLINE_DIALOG,$name"; }
elsif ($rc eq "6") { $command = "CHART_OBJECT_VLINE_DIALOG,$name"; }

# display chart object dialog
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update the app
$command = "CHART_OBJECT_UPDATE,$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
