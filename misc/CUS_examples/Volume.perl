# qtstalker Volume (Volume) indicator, red is down, blue is neutral, green is up, color based on close price.

$|++; # turn on autoflush, qtstalker needs this to know when data is finished being sent/recieved

print STDOUT "REF,Volume,Volume,0";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "REF,Close,Close,0"; # get close data
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "REF,PClose,Close,1"; # get close data offset -1 (returns previous days close)
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

print STDOUT "COMPARE,comp1,Close,PClose,>"; # return true if Close > PClose
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# color the Volume data green if comp1 == true
# we want to color up days green
print STDOUT "COLOR,comp1,Volume,1,green";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}


print STDOUT "COMPARE,comp2,Close,PClose,="; # return true if Close == PClose
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}

# color the Volume data blue if comp2 == true
# we want to color neutral days blue
print STDOUT "COLOR,comp2,Volume,1,blue";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}


print STDOUT "PLOT,Volume,V,red,Histogram Bar";
$a = <STDIN>; # get the return code
chomp($a); # remove the \n
if ($a ne "0")
{
  exit; # we have a non zero return code, exit script
}



