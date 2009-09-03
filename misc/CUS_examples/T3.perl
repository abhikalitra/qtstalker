# qtstalker T3 (Triple Exponential Moving Average) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "BARS,Bars,green,red,blue"; # create the BARS indicator
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "MA,T3,Close,5,T3"; # create the T3 line
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,Bars,C,red,Bar"; # plot BARS
$a = <STDIN>; # get the return code
chomp($a); # remove the \n 
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,T3,T3,yellow,Line"; # plot the T3
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

