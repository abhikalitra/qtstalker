#!/usr/bin/perl

# Example script to launch "Google Finance" in a browser window.

# Note: Handle your exchange below.
# Note: You might need a diffent "open" system command to launch your browser.

$|++;
my $DEBUG = 0;
if ($DEBUG) { doDebug("Start: $0"); }

if ($DEBUG) { doDebug("Get symbol"); }
print STDOUT "SYMBOL,CURRENT";
$rc = <STDIN>; chomp($rc);
if ($rc eq "ERROR") { doDebug("Failure: rc=$rc"); }
else { if ($DEBUG) { doDebug("Success: rc=$rc"); } }

my ($exchange, $symbol) = split(/,/, $rc);
my $exchangeG = $exchange;
CASE_EXCHANGE:
{
  if ($exchange eq "XASX") {
    $exchangeG = "ASX";
    last CASE_EXCHANGE;
  } 

  # catchall
  doDebug("Google exchange not known: $exchange");
}

if ($DEBUG) { doDebug("Launching ..."); }
my $url = "http://www.google.com/finance?q=" . $exchangeG . ":" . $symbol;
$rc = system("/usr/bin/open $url");
if ($rc ne "0") { die "System call failed: $?"; }

if ($DEBUG) { doDebug("End launch"); }

sub doDebug {
  my ($msg) = @_;
  print STDERR "DEBUG: $msg";
}
