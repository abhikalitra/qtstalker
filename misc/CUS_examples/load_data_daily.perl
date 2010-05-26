#!/usr/bin/perl

# Load the end-of-day data.

# NOTE: It will look for data files in "incoming" directory, process, and load.
# These temporary datafiles will be deleted after loading.

$|++;
my $DEBUG = 0;
if ($DEBUG) { doDebug("Start: $0"); }

my $exchange = "XASX";
my $incomingDir= $ENV{HOME} . "/Documents/trading/data/incoming/qtstalker";

my $quoteStrBase = "QUOTE,Stock,SET_QUOTE,$exchange,";

# ensure that there are files to process
opendir(INCOMING_DIR, $incomingDir)
  or die "Could not open directory '$incomingDir': $!";
my @txtFiles = grep(/pp[0-9]+\.txt/, readdir(INCOMING_DIR));
closedir(INCOMING_DIR);
if ($#txtFiles < 0) {
  print STDERR "No files are ready to be processed\n";
  exit;
}
else {
  if ($DEBUG) {
    my $msg = sprintf("Found %d files to be processed ...", $#txtFiles+1);
    doDebug($msg);
    foreach $file (sort @txtFiles) {
      doDebug($file);
    }
  }
}

foreach my $file (sort @txtFiles) {
  if ($DEBUG) { doDebug("Process $file"); }
  my $inFile = $incomingDir . "/" . $file;
  open (INPUT, "<$inFile") or die "Could not open input file '$inFile': $!";
  while (<INPUT>) {
    next if (/^#/);
    next if (/^$/);
    chomp;
    my ($asset, $quote) = split(/,/, $_, 2);
    my $quoteStr = $quoteStrBase . $asset . ",yyyyMMdd," . $quote;
    print STDOUT $quoteStr;
    my $rc = <STDIN>; chomp($rc);
    if ($rc ne "0") { if ($DEBUG) { doDebug("Failure: rc=$rc"); } }
  }
  close INPUT;
}

if ($DEBUG) { doDebug("Finalise"); }
print STDOUT "QUOTE,Stock,SAVE_QUOTES";
$rc = <STDIN>; chomp($rc);
if ($rc) { doDebug( "Failure: rc=$rc"); }
else {
  if ($DEBUG) { doDebug("Success: rc=$rc"); }
  foreach my $file (@txtFiles) {
    my $inFile = $incomingDir . "/" . $file;
    unlink $inFile;
  }
}

sub doDebug {
  my ($msg) = @_;
  print STDERR "DEBUG: $msg\n";
}
