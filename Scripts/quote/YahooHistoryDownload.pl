# download yahoo historical quotes

$|=1;

# display the yahoo dialog
$command = "PLUGIN=YAHOO_DIALOG";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the chart panel
$command = "PLUGIN=CONTROL_PANEL,METHOD=CHART_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
