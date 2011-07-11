# qtstalker test indicator

$closeName = 'Close';

$| = 1;
use File::Basename;

my $DEBUG = 1; # 0..1
my $scriptName = basename($0);
if ($DEBUG) { doDebug('Start'); }

if ($DEBUG) { doDebug("Get $closeName"); }
$command = "PLUGIN=DOHLCVI,METHOD=C,NAME_CLOSE=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq 'ERROR') { doError($command); }

# Get the index ranges for the close
if ($DEBUG) { doDebug('Get the range'); }
$command = "PLUGIN=INDICATOR,METHOD=INDEX_RANGE,NAME=$closeName";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq 'ERROR') { doError($command); }

# start
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_RANGE_START";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq 'ERROR') { doError($command); }
my $rangeStart = $rc;

# end
$command = "PLUGIN=SCRIPT_RETURN_DATA,KEY=INDICATOR_RANGE_END";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq 'ERROR') { doError($command); }
my $rangeEnd = $rc;

doDebug("rangeStart=$rangeStart rangeEnd=$rangeEnd");

# set the plot style
if ($DEBUG) { doDebug('Do Plot'); }
print STDOUT "PLUGIN=INDICATOR,METHOD=PLOT_ALL,NAME=$closeName,STYLE=Line,COLOR=yellow,Z=1";
$rc = <STDIN>; chomp($rc); if ($rc ne '0') { exit; }

if ($DEBUG) { doDebug('End'); }

sub doDebug {
  my ($msg) = @_;
  print STDERR "DEBUG: $scriptName: $msg\n";
}

sub doWarn {
  my ($msg) = @_;
  print STDERR "WARN: $scriptName: $msg\n";
}

sub doError {
  my ($msg) = @_;
  print STDERR "ERROR: $scriptName: $msg\n";
  exit;
}
