# qtstalker c_kk - Candles: Piercing Line (Kirikomi) and Dark Cloud Cover (Kabuse)

# green : Piercing Line
# red   : Dark Cloud Cover

$|++;

# get the Candles
print STDOUT "INDICATOR,PLUGIN,CANDLES,NONE,candles,dimgray";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# set the color
print STDOUT "INDICATOR,SET_COLOR_ALL,candles,dimgray";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Kirikomi
print STDOUT "INDICATOR,PLUGIN,CANDLES,PIERCING,ckk1,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get Kabuse
print STDOUT "INDICATOR,PLUGIN,CANDLES,DARKCLOUDCOVER,ckk2,0";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# get the index range of the piercing bars
print STDOUT "INDICATOR,GET_RANGE,ckk1";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }

# split the start and end values
my @range = split(',', $rc);

for ($count = $range[0]; $count <= $range[1]; $count++)
{
  # get the kirikomi value
  print STDOUT "INDICATOR,GET_INDEX,ckk1,$count";
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { next; } # empty index position, continue

  if ($rc >= 1)
  {
    # set the current bar color to green
    print STDOUT "INDICATOR,SET_COLOR,candles,$count,green";
    $rc = <STDIN>; chomp($rc); if ($rc ne "0") { next; }
  }

  # get the kabuse value
  print STDOUT "INDICATOR,GET_INDEX,ckk2,$count";
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { next; } # empty index position, continue

  if ($rc <= -1)
  {
    # set the current bar color to red
    print STDOUT "INDICATOR,SET_COLOR,candles,$count,red";
    $rc = <STDIN>; chomp($rc); if ($rc ne "0") { next; }
  }
}

# plot the candles
print STDOUT "INDICATOR,SET_PLOT,candles";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

