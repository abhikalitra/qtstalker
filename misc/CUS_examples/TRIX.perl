# qtstalker TRIX (1-day Rate-Of-Change (ROC) of a Triple Smooth EMA) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "INDICATOR,TRIX,TRIX,Close,30"; # create the TRIX line
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,TRIX,TRIX,red,Histogram Bar"; # plot the TRIX
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

