# qtstalker Volume (Volume) indicator, red is down, blue is neutral, green is up, color based on close price.

$|++;

# get the volume data
print STDOUT "INDICATOR,NEW,Volume,Volume";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# get close data
print STDOUT "INDICATOR,NEW,Close,cl";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# get the index range of the close bars
print STDOUT "INDICATOR,GET_RANGE,cl";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# split the start and end values
my @range = split(',', $rc);

$count = $range[0];
$pcount = $range[0];
$pcount--;

for (; $count <= $range[1]; $count++, $pcount++)
{
  # get the current close value
  print STDOUT "INDICATOR,GET_INDEX,cl,$count";
  $close = <STDIN>; chomp($close); if ($close eq "ERROR") { next; } # empty index position, continue

  # get the yesterdays close value
  print STDOUT "INDICATOR,GET_INDEX,cl,$pcount";
  $yclose = <STDIN>; chomp($yclose); if ($yclose eq "ERROR") { next; } # empty index position, continue

  if ($close > $yclose)
  {
    # set the current volume bar color to green
    print STDOUT "INDICATOR,SET_COLOR,Volume,$count,green";
    $rc = <STDIN>; chomp($rc); if ($rc ne "0") { next; }
  }
  else
  {
    if ($close < $yclose)
    {
      # set the current bar color to red
      print STDOUT "INDICATOR,SET_COLOR,Volume,$count,red";
      $rc = <STDIN>; chomp($rc); if ($rc ne "0") { next; }
    }
    else
    {
      # set the current bar color to blue
      print STDOUT "INDICATOR,SET_COLOR,Volume,$count,blue";
      $rc = <STDIN>; chomp($rc); if ($rc ne "0") { next; }
    }
  }
}

# set the plot style to Histogram Bar
print STDOUT "INDICATOR,SET_PLOT_STYLE,Volume,Histogram Bar";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

# plot the Volume indicator
print STDOUT "INDICATOR,SET_PLOT,Volume";
$a = <STDIN>; chomp($a); if ($a ne "0") { exit; }

