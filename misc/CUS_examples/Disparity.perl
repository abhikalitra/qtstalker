# qtstalker Disparity Index indicator
# See: http://tadoc.org/indicator/DISPARITY.htm

$|++;

# Get today's close
print STDOUT "INDICATOR,NEW,Close,cl,Line,red";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# Get the 13-bar SMA
print STDOUT "INDICATOR,PLUGIN,MA,SMA,sma_13,Close,13,Line,red";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# create the disparity indicator
print STDOUT "INDICATOR,NEW,EMPTY,Disparity,Histogram Bar,red";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get the index range of the close
print STDOUT "INDICATOR,GET_RANGE,cl";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# split the start and end values
my @range = split(',', $rc);

for ($count = 12; $count <= $range[1]; $count++)
{
  # get the current close value
  print STDOUT "INDICATOR,GET_INDEX,cl,$count";
  $close = <STDIN>; chomp($close); if ($close eq "ERROR") { next; } # empty index position, continue

  # get sma_13 value
  print STDOUT "INDICATOR,GET_INDEX,sma_13,$count";
  $ma = <STDIN>; chomp($ma); if ($ma eq "ERROR") { next; } # empty index position, continue

  $disparity = sprintf("%.2f", ($close - $ma) / $ma * 100);

  # set the disparity indicator with value
  print STDOUT "INDICATOR,SET_INDEX,Disparity,$count,$disparity,red";
  $rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
}

print STDOUT "PLOT,Disparity";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

