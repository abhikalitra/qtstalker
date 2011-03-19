########################################################################################################
# exports symbols in the database to a CSV files
#
########################################################################################################


###############################################################################################################

$|++;

# display input dialog
$s1 = "PLUGIN=INPUT_DIALOG,TITLE=Export Symbols To CSV File";
$s2 = "INPUT1_LABEL=Symbols,INPUT1_TYPE=SYMBOL";
$s3 = "INPUT2_LABEL=Export Directory,INPUT2_TYPE=FILE";
$command = "$s1,$s2,$s3";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the symbol string
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT1_DATA";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

my @symbols = split(';', $rc);

# get the export file
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INPUT_DIALOG_INPUT2_DATA";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

my @dirs = split(';', $rc);
$dir = $dirs[0];

foreach $item (@symbols)
{
  my @symbol = split(':', $item);

  $command = "PLUGIN=QUOTE_DATABASE_DUMP,DIRECTORY=$dir,EXCHANGE=$symbol[0],SYMBOL=$symbol[1]";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }
}
