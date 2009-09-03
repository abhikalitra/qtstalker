# qtstalker LINEARREG_ANGLE (Linear Regression Angle) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "LINEARREG_ANGLE,LINEARREG_ANGLE,Close,14"; # create the LINEARREG_ANGLE line
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,LINEARREG_ANGLE,LRA,red,Histogram Bar"; # plot the LINEARREG_ANGLE
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

