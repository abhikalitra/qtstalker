# qtstalker Close indicator

$chartName = "CL";

$| = 1;
use File::Basename;

my $DEBUG = 0; # 0..1
my $scriptName = basename($0);
if ($DEBUG) {
  my $outFile = "/tmp/qtstalker_log_" . $scriptName . ".txt";
  open(LOGFILE, ">$outFile")
    or die "Could not open output file '$outFile': $!";
  select((select(LOGFILE), $| = 1)[0]);
  $runTime = `date`; chomp($runTime);
  doDebug("Start: $runTime");
}

# Configuration:
my $command = '';
my $c0Data = 'close';
my $c0Name = 'Close';
my $c0Label = 'Close';
my $c0Style = 'Line';
my $c0Z = '0';
my $c0Pen = '1';
my $c0Colour = 'yellow';

# create the chart object
$command = "NEW(CHART, chart)";
if ($DEBUG) { doDebug("$command"); }
print STDOUT $command;
$rc = <STDIN>; chomp($rc);
if ($rc eq 'ERROR') { if ($DEBUG) { doDebug('ERROR'); } doExit(); }

# specify some settings
$command = "SET(chart.NAME, $chartName,
                chart.ROW, 0,
                chart.COL, 0,
                chart.DATE, 1,
                chart.LOG, 0)";
if ($DEBUG) { doDebug("$command"); }
print STDOUT $command;
$rc = <STDIN>; chomp($rc);
if ($rc eq 'ERROR') { if ($DEBUG) { doDebug('ERROR'); } doExit(); }

# create the chart
$command = "RUN(chart)";
if ($DEBUG) { doDebug("$command"); }
print STDOUT $command;
$rc = <STDIN>; chomp($rc);
if ($rc eq 'ERROR') { if ($DEBUG) { doDebug('ERROR'); } doExit(); }

# create the SYMBOL_CURRENT object
$command = "NEW(SYMBOL_CURRENT, symbol_current)";
if ($DEBUG) { doDebug("$command"); }
print STDOUT $command;
$rc = <STDIN>; chomp($rc);
if ($rc eq 'ERROR') { if ($DEBUG) { doDebug('ERROR'); } doExit(); }

# no need to set any values, use the defaults

# load the quotes
$command = "RUN(symbol_current)";
if ($DEBUG) { doDebug("$command"); }
print STDOUT $command;
$rc = <STDIN>; chomp($rc);
if ($rc eq 'ERROR') { if ($DEBUG) { doDebug('ERROR'); } doExit(); }

# create the PLOT_LINE object
$command = "NEW(PLOT_LINE, plot_line)";
if ($DEBUG) { doDebug("$command"); }
print STDOUT $command;
$rc = <STDIN>; chomp($rc);
if ($rc eq 'ERROR') { if ($DEBUG) { doDebug('ERROR'); } doExit(); }

# set the attributes
$command = "SET(plot_line.OUTPUT, $c0Name,
                plot_line.CHART, $chartName,
                plot_line.LABEL, $c0Label,
                plot_line.STYLE, $c0Style,
                plot_line.COLOR, $c0Colour,
                plot_line.Z, $c0Z,
                plot_line.PEN, $c0Pen,
                plot_line.INPUT, $c0Data)";
if ($DEBUG) { doDebug("$command"); }
print STDOUT $command;
$rc = <STDIN>; chomp($rc);
if ($rc eq 'ERROR') { if ($DEBUG) { doDebug('ERROR'); } doExit(); }

# add line to the chart
$command = "RUN(plot_line)";
if ($DEBUG) { doDebug("$command"); }
print STDOUT $command;
$rc = <STDIN>; chomp($rc);
if ($rc eq 'ERROR') { if ($DEBUG) { doDebug('ERROR'); } doExit(); }

# create the CHART_UPDATE object
$command = "NEW(CHART_UPDATE, chart_update)";
if ($DEBUG) { doDebug("$command"); }
print STDOUT $command;
$rc = <STDIN>; chomp($rc);
if ($rc eq 'ERROR') { if ($DEBUG) { doDebug('ERROR'); } doExit(); }

# set the values
$command = "SET(chart_update.CHART, $chartName, chart_update.DATE, date)";
if ($DEBUG) { doDebug("$command"); }
print STDOUT $command;
$rc = <STDIN>; chomp($rc);
if ($rc eq 'ERROR') { if ($DEBUG) { doDebug('ERROR'); } doExit(); }

# update the chart
$command = "RUN(chart_update)";
if ($DEBUG) { doDebug("$command"); }
print STDOUT $command;
$rc = <STDIN>; chomp($rc);
if ($rc eq 'ERROR') { if ($DEBUG) { doDebug('ERROR'); } doExit(); }

if ($DEBUG) { doDebug("End"); }
doExit();

sub doDebug {
  my ($msg) = @_;
  #sleep 2;
  print LOGFILE "$msg\n";
}

sub doExit {
  if ($DEBUG) { close LOGFILE; }
  exit;
}
