# qtstalker BETA (Beta) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

# create the BETA indicator
print STDOUT "BETA,BETA,SYMBOL,Close,5";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# plot the BETA indicator
print STDOUT "PLOT,BETA,BETA,red,Histogram Bar";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n 
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

