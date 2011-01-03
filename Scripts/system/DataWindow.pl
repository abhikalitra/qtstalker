# plot font dialog

$|=1;

# get current indicators
$command = "INDICATOR_DATABASE,INDICATORS";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

# split the start and end values
my @list = split(',', $rc);

# create the data window
$command = "DATA_WINDOW,START";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }

foreach my $item (@list)
{
  # create the BARS indicator
  $command = "INDICATOR_RUN,$item";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; next; }

  # display the font dialog
  $command = "DATA_WINDOW,SET";
  print STDOUT $command;
  $rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; next; }
}

# show the data window
$command = "DATA_WINDOW,END";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") { print STDERR $command; exit; }
