# qtstalker ROCR (Rate Of Change Ratio) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "ROCR,ROCR,Close,10"; # create the ROCR line
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,ROCR,ROCR,red,Histogram Bar"; # plot the ROCR
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

