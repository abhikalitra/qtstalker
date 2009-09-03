# qtstalker HT_SINE (Hilbert Transform SineWave) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "HT_SINE,Close,SINE,LEAD_SINE"; # create the HT_SINE lines
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,SINE,Sine,red,Line"; # plot the LEAD
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,LEAD_SINE,LSine,yellow,Line"; # plot the LEAD_SINE
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}



