# qtstalker Disparity Index indicator
# See: http://tadoc.org/indicator/DISPARITY.htm

$closeName = 'cl';

$name = 'Disparity';
$style = 'Histogram Bar';
$color = 'yellow';
$period = 13;

$maType = 'SMA';
$maName = 'sma_13';

########################################################################

$|++;

# Get today's close
$command = "BARS,Close,$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# Get the 13-bar SMA
$command = "MA,$maType,$maName,$closeName,$period";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# create the disparity indicator
$command = "INDICATOR_PLOT_NEW,$name";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# get the index range of the close bars
$command = "INDICATOR_PLOT_INDEX_RANGE,$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# split the start and end values
my @range = split(',', $rc);

for ($count = 12; $count <= $range[1]; $count++)
{
  # get the current close value
  $command = "INDICATOR_PLOT_INDEX_GET,$closeName,$count";
  print STDOUT $command;
  $close = <STDIN>; chomp($close); if ($close eq "ERROR") { print STDERR $command; next; } # empty index position, continue

  # get the ma value
  $command = "INDICATOR_PLOT_INDEX_GET,$maName,$count";
  print STDOUT $command;
  $ma = <STDIN>; chomp($ma); if ($ma eq "ERROR") { print STDERR $command; next; } # empty index position, continue

  $disparity = sprintf("%.2f", ($close - $ma) / $ma * 100);

  # set the disparity indicator with value
  $command = "INDICATOR_PLOT_INDEX_SET,$name,$count,$disparity,red";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
}

$command = "INDICATOR_PLOT_ALL,$name,$style,$color,0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
