# qtstalker Disparity Index indicator
# See: http://tadoc.org/indicator/DISPARITY.htm

$|++;

# Get today's close
$command = "BARS,Close,cl";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# Get the 13-bar SMA
$command = "MA,SMA,sma_13,cl,13";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the disparity indicator
$command = "INDICATOR_PLOT_NEW,Disparity";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the index range of the close bars
$command = "INDICATOR_PLOT_INDEX,RANGE,cl";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# split the start and end values
my @range = split(',', $rc);

for ($count = 12; $count <= $range[1]; $count++)
{
  # get the current close value
  $command = "INDICATOR_PLOT_INDEX,GET,cl,$count";
  print STDOUT $command;
  $close = <STDIN>; chomp($close); if ($close eq "ERROR") { print STDERR $command; next; } # empty index position, continue

  # get sma_13 value
  $command = "INDICATOR_PLOT_INDEX,GET,sma_13,$count";
  print STDOUT $command;
  $ma = <STDIN>; chomp($ma); if ($ma eq "ERROR") { print STDERR $command; next; } # empty index position, continue

  $disparity = sprintf("%.2f", ($close - $ma) / $ma * 100);

  # set the disparity indicator with value
  $command = "INDICATOR_PLOT_INDEX,SET,Disparity,$count,$disparity,red";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
}

# set the plot style
$command = "INDICATOR_PLOT_STYLE,Disparity,Histogram Bar";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

#set the color
$command = "INDICATOR_PLOT_COLOR,ALL,Disparity,yellow";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

#plot the disparity
$command = "INDICATOR_PLOT,Disparity,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
