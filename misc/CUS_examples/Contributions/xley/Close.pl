# qtstalker Close indicator

use File::Basename;

$| = 1;
my $DEBUG = 0; # 0..1
my $scriptName = basename($0);
if ($DEBUG) { doDebug('Start'); }

if ($DEBUG) { doDebug('Get close_0'); }
print STDOUT 'PLUGIN=DOHLCVI,METHOD=C,NAME=close_0';
$rc = <STDIN>; chomp($rc); if ($rc ne '0') { exit; }

# set the plot style
if ($DEBUG) { doDebug('Do Plot'); }
print STDOUT 'PLUGIN=INDICATOR,METHOD=PLOT_ALL,NAME=close_0,STYLE=Line,COLOR=yellow,Z=1';
$rc = <STDIN>; chomp($rc); if ($rc ne '0') { exit; }

if ($DEBUG) { doDebug('End'); }

sub doDebug {
  my ($msg) = @_;
  print STDERR "DEBUG: $scriptName: $msg\n";
}
