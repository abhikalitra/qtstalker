# qtstalker day 20SMA 50SMA 200SMA ( day, 20 period SMA, 50 period SMA, 200 period SMA) indicator
# created 13-09-2009 x_curve
# colors are in #xxxxxx format or in plain english
#plot the chart in this order 200, 50, 20, day so day overlays the others

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved


# create the day indicator
print STDOUT "INDICATOR,REF,Day,Close,0";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# create the 20 SMA indicator
print STDOUT "INDICATOR,MA,SMA,20MA,Close,20";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# create the 50 SMA indicator
print STDOUT "INDICATOR,MA,SMA,50MA,Close,50";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# create the 200 SMA indicator
print STDOUT "INDICATOR,MA,SMA,200MA,Close,200";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# plot 200 SMA, color ligth yellow,
# print 200SMA - 50SMA - 20SMA - Day, day is on the foreground
print STDOUT "PLOT,200MA,200MA,#f3ff88,Dot";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

#plot 50 SMA, color purple
print STDOUT "PLOT,50MA,50MA,#9041ff,Dash";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# plot 20 SMA, color green
print STDOUT "PLOT,20MA,20MA,#09ff00,Dash";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# plot day, color red
print STDOUT "PLOT,Day,Day,Red,Line";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}
