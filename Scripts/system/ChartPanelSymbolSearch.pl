# display symbols in the chart panel from the symbol dialog

$|=1;

# display the symbol search dialog
$command = "SYMBOL_DIALOG,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# split the start and end values
my @search = split(',', $rc);

# get symbols selected from the chart panel
$command = "CHART_PANEL_SEARCH,$search[0],$search[1]";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# refresh the chart panel
$command = "CHART_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
