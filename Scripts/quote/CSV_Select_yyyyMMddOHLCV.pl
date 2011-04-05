# import quotes from a CSV file with filename as symbol.
# format is yyyyMMdd,open,high,low,close,volume

$|=1;
use File::Basename;
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

# get the filename from the path
$symbol = basename($file);

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
    if ($count lt '6')
    {
      next;
    }

    $command = "PLUGIN=QUOTE_DATABASE,METHOD=SET,EXCHANGE=$exchange,SYMBOL=$symbol,DATE_FORMAT=$dateFormat,DATE=$columns[0],OPEN=$columns[1],HIGH=$columns[2],LOW=$columns[3],CLOSE=$columns[4],VOLUME=$columns[5]";
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
