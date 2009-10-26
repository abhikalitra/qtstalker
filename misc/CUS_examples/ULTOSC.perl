# qtstalker ULTOSC (Ultimate Oscillator) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "INDICATOR,ULTOSC,ULTOSC,7,14,28"; # create the ULTOSC line
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,ULTOSC,ULTOSC,red,Line"; # plot the ULTOSC
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

