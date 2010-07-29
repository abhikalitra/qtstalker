# qtstalker CandleRelative indicator
# Coloured relative to previous Close.

$|++;

# get the candles
print STDOUT "INDICATOR,PLUGIN,CANDLES,NONE,candles";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set candles color
print STDOUT "INDICATOR,SET_COLOR_ALL,candles,dimgray";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Close
print STDOUT "INDICATOR,NEW,Close,cl";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

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
  if ($pcount == -1) { next; }
  else
  {
    print STDOUT "INDICATOR,GET_INDEX,cl,$pcount";
    $yclose = <STDIN>; chomp($yclose); if ($yclose eq "ERROR") { next; } # empty index position, continue
  }

  if ($close > $yclose)
  {
    # set the current bar color to green
    print STDOUT "INDICATOR,SET_COLOR,candles,$count,green";
    $rc = <STDIN>; chomp($rc); if ($rc ne "0") { next; }
  }
  else
  {
    if ($close < $yclose)
    {
      # set the current bar color to red
      print STDOUT "INDICATOR,SET_COLOR,candles,$count,red";
      $rc = <STDIN>; chomp($rc); if ($rc ne "0") { next; }
    }
    else
    {
      # set the current bar color to blue
      print STDOUT "INDICATOR,SET_COLOR,candles,$count,blue";
      $rc = <STDIN>; chomp($rc); if ($rc ne "0") { next; }
    }
  }
}

print STDOUT "INDICATOR,SET_PLOT,candles";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

