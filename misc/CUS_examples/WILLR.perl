# qtstalker WILLR (Williams' %R) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "WILLR,WILLR,14"; # create the WILLR line
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,WILLR,WILLR,red,Line"; # plot the WILLR
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

