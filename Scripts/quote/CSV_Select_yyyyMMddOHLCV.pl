# import quotes from a CSV file with filename as symbol.
# format is yyyyMMdd,open,high,low,close,volume

$|=1;
use File::Basename;
use Text::CSV;

$exchange = 'XNYS';
$dateFormat = 'yyyyMMdd';

################################################################################

# display the file selection dialog
$command = "PLUGIN=FILE_DIALOG,MODE=0,FORMAT=0";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the filename 
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=FILE_DIALOG_FILE";
print STDOUT $command;
$file = <STDIN>; chomp($file); if ($file eq "ERROR") { print STDERR $command; next; }

# get the filename from the path
$symbol = basename($file);

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

    $command = "PLUGIN=QUOTE_DATABASE_SET,EXCHANGE=$exchange,SYMBOL=$symbol,DATE_FORMAT=$dateFormat,DATE=$columns[0],OPEN=$columns[1],HIGH=$columns[2],LOW=$columns[3],CLOSE=$columns[4],VOLUME=$columns[5]";
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
$command = "PLUGIN=QUOTE_DATABASE_SET,SAVE=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

# refresh qtstalker chart panel
$command = "PLUGIN=CHART_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
