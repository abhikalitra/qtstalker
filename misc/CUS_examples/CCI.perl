# qtstalker CCI (Commodity Channel Index) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

# create the CCI indicator
print STDOUT "CCI,CCI,14";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# plot the CCI indicator
print STDOUT "PLOT,CCI,CCI,red,Line";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n 
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

