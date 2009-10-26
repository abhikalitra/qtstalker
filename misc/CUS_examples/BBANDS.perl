# qtstalker BBANDS (Bollinger Bands) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

# create the BARS indicator
print STDOUT "INDICATOR,BARS,Bars,green,red,blue"; # get the BARS indicator
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# plot the BARS indicator
print STDOUT "PLOT,Bars,C,red,Bar";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n 
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# create the BBANDS indicator
print STDOUT "INDICATOR,BBANDS,Close,UPPER,MIDDLE,LOWER,5,2,2,SMA";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# plot the UPPER BBANDS line
print STDOUT "PLOT,UPPER,UPPER,red,Line";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# plot the MIDDLE BBANDS line
print STDOUT "PLOT,MIDDLE,MIDDLE,gray,Line";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# plot the LOWER BBANDS line
print STDOUT "PLOT,LOWER,LOWER,red,Line";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

