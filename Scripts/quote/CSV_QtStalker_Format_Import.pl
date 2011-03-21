########################################################################################
# import symbols that were previously exported by QtStalker Export function
#
##########################################################################################

$|=1;

use Text::CSV;

$dateFormat = 'yyyy-MM-ddTHH:mm:ss';

################################################################################

# display the file selection dialog
$command = "PLUGIN=FILE_DIALOG,MODE=1,TITLE=Select QtStalker CSV File";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# get the filenames
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=FILE_DIALOG_FILE";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

my @files = split(';', $rc);

# create the CSV object
my $csv = Text::CSV->new();

foreach $file (@files)
{
  # open the csv file
  open (CSV, "<", $file) or die $!;

  # read one line at a time and parse
  while (<CSV>)
  {
    if ($csv->parse($_))
    {
      my @columns = $csv->fields();
      $count = @columns;
      if ($count ne '10')
      {
        next;
      }

      $command = "PLUGIN=QUOTE_DATABASE_SET,EXCHANGE=$columns[0],SYMBOL=$columns[1],NAME=$columns[2],DATE_FORMAT=$dateFormat,DATE=$columns[3],OPEN=$columns[4],HIGH=$columns[5],LOW=$columns[6],CLOSE=$columns[7],VOLUME=$columns[8],INTEREST=$columns[9]";
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
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
}

# refresh qtstalker chart panel
$command = "PLUGIN=CHART_PANEL_REFRESH";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
