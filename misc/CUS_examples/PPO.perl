# qtstalker PPO (Percentage Price Oscillator) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "INDICATOR,PPO,PPO,Close,12,26,SMA"; # create the PPO line
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,PPO,PPO,red,Histogram Bar"; # plot the PPO
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

