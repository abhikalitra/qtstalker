# qtstalker LINEARREG_SLOPE (Linear Regression Slope) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "LINEARREG_SLOPE,LINEARREG_SLOPE,Close,14"; # create the LINEARREG_SLOPE line
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,LINEARREG_SLOPE,LRS,red,Histogram Bar"; # plot the LINEARREG_SLOPE
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

