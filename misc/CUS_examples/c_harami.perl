# qtstalker c_harami - Candles: Harami and Harami Cross

# green   : Bull Harami
# cyan    : Bull Harami Cross
# red     : Bear Harami
# magenta : Bear Harami Cross

$|++;

# get the Candles
print STDOUT "INDICATOR,PLUGIN,CANDLES,NONE,candles";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set the color
print STDOUT "INDICATOR,SET_COLOR_ALL,candles,dimgray";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Harami
print STDOUT "INDICATOR,PLUGIN,CANDLES,HARAMI,ch1,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Harami Cross
print STDOUT "INDICATOR,PLUGIN,CANDLES,HARAMICROSS,ch2,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get the index range of the harami bars
print STDOUT "INDICATOR,GET_RANGE,ch1";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# split the start and end values
my @range = split(',', $rc);

for ($count = $range[0]; $count <= $range[1]; $count++)
{
  # get the harami value
  print STDOUT "INDICATOR,GET_INDEX,ch1,$count";
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { next; } # empty index position, continue

  if ($rc >= 1)
  {
    # set the current bar color to green
    print STDOUT "INDICATOR,SET_COLOR,candles,$count,green";
    $rc = <STDIN>; chomp($rc); if ($rc ne "0") { next; }
  }
  else
  {
    if ($rc <= -1)
    {
      # set the current bar color to red
      print STDOUT "INDICATOR,SET_COLOR,candles,$count,red";
      $rc = <STDIN>; chomp($rc); if ($rc ne "0") { next; }
    }
  }

  # get the harami cross value
  print STDOUT "INDICATOR,GET_INDEX,ch2,$count";
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { next; } # empty index position, continue

  if ($rc >= 1)
  {
    # set the current bar color to green
    print STDOUT "INDICATOR,SET_COLOR,candles,$count,cyan";
    $rc = <STDIN>; chomp($rc); if ($rc ne "0") { next; }
  }
  else
  {
    if ($rc <= -1)
    {
      # set the current bar color to red
      print STDOUT "INDICATOR,SET_COLOR,candles,$count,magenta";
      $rc = <STDIN>; chomp($rc); if ($rc ne "0") { next; }
    }
  }
}

# plot the candles 
print STDOUT "INDICATOR,SET_PLOT,candles";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
