# qtstalker BOP indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

# create the BOP indicator
print STDOUT "BOP,BOP";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# plot the BOP indicator
print STDOUT "PLOT,BOP,BOP,red,Histogram";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n 
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

