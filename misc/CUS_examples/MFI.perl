# qtstalker MFI (Money Flow Index) indicator

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "MFI,MFI,14"; # create the MFI line
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "PLOT,MFI,MFI,red,Line"; # plot the MFI
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

