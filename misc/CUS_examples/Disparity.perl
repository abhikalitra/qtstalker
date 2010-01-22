# qtstalker Disparity Index indicator
# See: http://tadoc.org/indicator/DISPARITY.htm

$|++;

# Get today's close
print STDOUT "GET_INDICATOR,Close,-1";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
@close_0 = split(",", $rc);

# Get the 13-bar SMA
print STDOUT "INDICATOR,SMA,sma_13,Close,13";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
print STDOUT "GET_INDICATOR,sma_13";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
@sma_13 = split(",", $rc);

# Get the 200-bar SMA
print STDOUT "INDICATOR,SMA,sma_200,Close,200";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
print STDOUT "GET_INDICATOR,sma_200";
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { exit; }
@sma_200 = split(",", $rc);

# Calculate the index values
my @values = ();
my $value = 0;
my $periodUnstable = 13;
for (my $i = ($periodUnstable - 1); $i <= $#close_0; $i++)
{
  my $a = @sma_13[$i - ($periodUnstable - 1)];
  if ($a != 0)
  {
    $value = sprintf("%.2f", (@close_0[$i] - $a) / $a * 100);
  }
  push(@values, $value);
}

printf STDOUT "SET_INDICATOR,disparity_13," . join(",", @values);
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,disparity_13,Disparity-13,red,Line";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

# Calculate the 200-bar index
@values = ();
$value = 0;
$periodUnstable = 200;
for (my $i = ($periodUnstable - 1); $i <= $#close_0; $i++)
{
  my $a = @sma_200[$i - ($periodUnstable - 1)];
  if ($a != 0)
  {
    $value = sprintf("%.2f", (@close_0[$i] - $a) / $a * 100);
  }
  push(@values, $value);
}

printf STDOUT "SET_INDICATOR,disparity_200," . join(",", @values);
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }

print STDOUT "PLOT,disparity_200,Disparity-200,orange,Line";
$rc = <STDIN>; chomp($rc); if ($rc ne "0") { exit; }
