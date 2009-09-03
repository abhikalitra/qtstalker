# qtstalker CORREL (Pearson's Correlation Coefficient) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "BARS,Bars,green,red,blue"; # get the BARS indicator
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "CORREL,CORREL,High,Low,20"; # create the CORREL indicator
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# now we plot the BARS indicator
print STDOUT "PLOT,Bars,C,red,Bar";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n 
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,CORREL,CORREL,red,Line"; # plot the CORREL line
$a = <STDIN>; # get the return code
chomp($a); # remove the \n 
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

