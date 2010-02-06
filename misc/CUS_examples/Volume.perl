# qtstalker Volume (Volume) indicator, red is down, blue is neutral, green is up, color based on close price.

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "INDICATOR,REF,vol,Volume,0";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "INDICATOR,REF,cl,Close,0"; # get close data
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "INDICATOR,REF,pcl,Close,1"; # get close data offset -1 (returns previous days close)
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "INDICATOR,COMPARE,comp1,cl,pcl,>"; # return true if Close > PClose
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# color the Volume data green if comp1 == true
# we want to color up days green
print STDOUT "INDICATOR,COLOR,comp1,vol,1,green";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}


print STDOUT "INDICATOR,COMPARE,comp2,cl,pcl,="; # return true if Close == PClose
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# color the Volume data blue if comp2 == true
# we want to color neutral days blue
print STDOUT "INDICATOR,COLOR,comp2,vol,1,blue";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}


print STDOUT "PLOT,vol,V,red,Histogram Bar";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}



