# qtstalker 2MA_Crossover (OHLC Bars, 50 period SMA, 200 period SMA) indicator

$|++;

# create the BARS indicator
print STDOUT "INDICATOR,PLUGIN,BARS,Bars,green,red,blue";
$a = <STDIN>;
chomp($a);
if ($a ne "0")
{
  exit;
}

# get some close bars
print STDOUT "INDICATOR,PLUGIN,REF,cl,Close,0";
$a = <STDIN>;
chomp($a);
if ($a ne "0")
{
  exit;
}

# create the 50 SMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,SMA,50MA,cl,50";
$a = <STDIN>;
chomp($a);
if ($a ne "0")
{
  exit;
}

# create the 200 SMA indicator
print STDOUT "INDICATOR,PLUGIN,MA,SMA,200MA,cl,200";
$a = <STDIN>;
chomp($a);
if ($a ne "0")
{
  exit;
}



# plot the BARS line
print STDOUT "PLOT,Bars,C,OHLC";
$a = <STDIN>;
chomp($a);
if ($a ne "0")
{
  exit;
}



# color the 50MA line prior to plotting
print STDOUT "INDICATOR,PLUGIN,COLOR,All,50MA,blue";
$a = <STDIN>;
chomp($a);
if ($a ne "0")
{
  exit;
}

# plot the 50MA
print STDOUT "PLOT,50MA,50MA,Line";
$a = <STDIN>;
chomp($a);
if ($a ne "0")
{
  exit;
}



# color the 200MA line prior to plotting
print STDOUT "INDICATOR,PLUGIN,COLOR,All,200MA,red";
$a = <STDIN>;
chomp($a);
if ($a ne "0")
{
  exit;
}

# plot the 200MA
print STDOUT "PLOT,200MA,200MA,Line";
$a = <STDIN>;
chomp($a);
if ($a ne "0")
{
  exit;
}

