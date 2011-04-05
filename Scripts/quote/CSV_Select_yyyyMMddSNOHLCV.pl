# import quotes from a CSV file.
# format is yyyyMMdd,symbol,name,open,high,low,close,volume

$|=1;
use Text::CSV;

#$exchange = 'XXXX';
$dateFormat = 'yyyyMMdd';

################################################################################

# display the file selection dialog
$command = "PLUGIN=FILE_DIALOG,MODE=0,TITLE=Select CSV File";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the filename 
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=FILE_DIALOG_FILE";
print STDOUT $command;
$file = <STDIN>; chomp($file); if ($file eq "ERROR") { print STDERR $command; exit; }

# display the exchange search dialog
$command = "PLUGIN=EXCHANGE_SEARCH_DIALOG";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the exchange
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=EXCHANGE_SEARCH_DIALOG_SELECTED";
print STDOUT $command;
$exchange = <STDIN>; chomp($exchange); if ($exchange eq "ERROR") { print STDERR $command; exit; }

# check for empty string
if ($exchange eq "") { print STDERR $command; exit; }

# create the CSV object
my $csv = Text::CSV->new();

# open the csv file
open (CSV, "<", $file) or die $!;

# read one line at a time and parse
while (<CSV>)
{
  if ($csv->parse($_))
  {
    my @columns = $csv->fields();
    $count = @columns;
    if ($count lt '8')
    {
      next;
    }

    $command = "PLUGIN=QUOTE_DATABASE,METHOD=SET,EXCHANGE=$exchange,SYMBOL=$columns[1],DATE_FORMAT=$dateFormat,DATE=$columns[0],NAME=$columns[2],OPEN=$columns[3],HIGH=$columns[4],LOW=$columns[5],CLOSE=$columns[6],VOLUME=$columns[7]";
    print STDOUT $command;
    $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }
  }
  else
  {
    my $err = $csv->error_input;
    print STDERR "Failed to parse line: $err";
  }
}
close CSV;

# commit quotes to disk
$command = "PLUGIN=QUOTE_DATABASE,METHOD=SET,SAVE=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# refresh qtstalker chart panel
$command = "PLUGIN=CONTROL_PANEL,METHOD=CHART_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
