# qtstalker HT SINE indicator

$input = 'c';

$sineName = 'SINE';
$sineStyle = 'Line';
$sineColor = 'red';

$leadName = 'LEAD';
$leadStyle = 'Line';
$leadColor = 'yellow';

###################################################################

$|++;

$command = "BARS,Close,$input";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "HT_SINE,$input,$sineName,$leadName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$sineName,$sineStyle,$sineColor,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

$command = "INDICATOR_PLOT_ALL,$leadName,$leadStyle,$leadColor,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
