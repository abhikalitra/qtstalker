# qtstalker LINEARREG_INTERCEPT (Linear Regression Intercept) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "INDICATOR,BARS,Bars,green,red,blue"; # create the BARS indicator
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "INDICATOR,LINEARREG_INTERCEPT,LINEARREG_INTERCEPT,Close,14"; # create the LINEARREG_INTERCEPT line
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

print STDOUT "PLOT,LINEARREG_INTERCEPT,LRI,red,Line"; # plot the LINEARREG_INTERCEPT
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

