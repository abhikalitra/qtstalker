# qtstalker Candle indicator
$|++;

print STDOUT "INDICATOR,CANDLES,NONE,candles,green"; # get the Candle indicator
$a = <STDIN>;
chomp($a);
if ($a ne "0")
{
  exit;
}

print STDOUT "PLOT,candles,C,Candle";
$a = <STDIN>;
chomp($a);
if ($a ne "0")
{
  exit;
}

