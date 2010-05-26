#!/usr/bin/perl

# Do initial loading of historical data.

$|++;
my $DEBUG = 0;

# List of assets to load. Leave empty to do all.
my @assetList = ();
my @assetList = (IAU,MGX,SBM);

my $exchange = "XASX";
my $dataDir = $ENV{HOME} . "/Documents/trading/data";

if ($DEBUG) { doDebug("Start: $0"); }

my $quoteStrBase = "QUOTE,Stock,SET_QUOTE,$exchange,";

if ($#assetList < 0) {
  if ($DEBUG) { doDebug("Finding all assets ..."); }
  opendir(DATA_DIR, $dataDir) or die "Could not open directory '$dataDir': $!";
  my @txtFiles = grep(/^[A-Z]+$/, readdir(DATA_DIR));
  closedir(DATA_DIR);
  foreach my $file (sort @txtFiles) {
    push(@assetList, $file);
  }
  my $msg = sprintf("Found %i assets.", $#assetList + 1);
  if ($DEBUG) { doDebug($msg); }
}

foreach my $asset (@assetList) {
  if ($DEBUG) { doDebug("Loading: $asset"); }
  my $quoteStrLeader = $quoteStrBase . $asset . ",yyyyMMdd,";

  my $inFile = $dataDir . "/" . $asset;
  open (INPUT, "<$inFile") or die "Could not open input file '$inFile': $!";
  while (<INPUT>) {
    #next if not (/^2009/); # for testing
    chomp;
    my $quoteStr = $quoteStrLeader . $_;
    print STDOUT $quoteStr;
    my $rc = <STDIN>; chomp($rc);
    if ($rc) { doDebug("Failure: rc=$rc"); }
    #else { doDebug("Success: rc=$rc"); }
  }
  close INPUT;
}

if ($DEBUG) { doDebug("Finalise");}
print STDOUT "QUOTE,Stock,SAVE_QUOTES";
$rc = <STDIN>; chomp($rc);
if ($rc) { doDebug( "Failure: rc=$rc"); }
else { if ($DEBUG) { doDebug("Success: rc=$rc"); } }

sub doDebug {
  my ($msg) = @_;
  print STDERR "DEBUG: $msg\n";
}
