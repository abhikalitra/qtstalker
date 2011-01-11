# qtstalker ADXR indicator

$|++;

# create the ADX
$command = "ADX,ADX,14";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the ADXR
$command = "ADXR,ADXR,10";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set ADX plot style
$command = "INDICATOR_PLOT_STYLE,ADX,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set ADX color
$command = "INDICATOR_PLOT_COLOR,ALL,ADX,red";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the ADX
$command = "INDICATOR_PLOT,ADX,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set ADXR plot style
$command = "INDICATOR_PLOT_STYLE,ADXR,Line";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# set ADXR color
$command = "INDICATOR_PLOT_COLOR,ALL,ADXR,yellow";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# plot the ADXR
$command = "INDICATOR_PLOT,ADXR,1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
