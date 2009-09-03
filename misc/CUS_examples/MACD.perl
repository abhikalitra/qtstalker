# qtstalker MACD (Moving Average Convergence Divergence) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "MACD,Close,MACD,Signal,Hist,12,26,9"; # create the MACD indicator
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,Hist,Hist,red,Histogram"; # plot the Histogram
$a = <STDIN>; # get the return code
chomp($a); # remove the \n 
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,MACD,MACD,blue,Line"; # plot the MACD line
$a = <STDIN>; # get the return code
chomp($a); # remove the \n 
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,Signal,Signal,yellow,Line"; # plot the Signal line
$a = <STDIN>; # get the return code
chomp($a); # remove the \n 
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

