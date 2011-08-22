# qtstalker VOL indicator

###################################################################

$|++;

# create the chart
$command = "STEP=create chart;
            COMMAND=CHART;
            NAME=VOL2;
            DATE=false;
            LOG=false;
            ROW=1;
            COL=2";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# load current bars
$command = "STEP=load current symbol;
            COMMAND=SYMBOL_CURRENT;
            DATE=date;
            OPEN=open;
            HIGH=high;
            LOW=low;
            CLOSE=close;
            VOLUME=volume;
            OI=oi";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot the histogram
$command = "STEP=create vol;
            COMMAND=PLOT_HISTOGRAM_BAR;
            CHART=create chart:NAME;
            DATE=load current symbol:DATE;
            INPUT=load current symbol:VOLUME;
            NAME=VOL;
            COLOR=#0000ff;
            Z=0;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color up bars
$command = "STEP=color up vol;
            COMMAND=COLOR;
            NAME=load current symbol:CLOSE;
            NAME_OFFSET=0;
            NAME_2=load current symbol:CLOSE;
            NAME_2_OFFSET=1;
            OP=GT;
            NAME_3=create vol:NAME;
            NAME_3_OFFSET=0;
            COLOR=#00FF00";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color down bars
$command = "STEP=color down vol;
            COMMAND=COLOR;
            NAME=load current symbol:CLOSE;
            NAME_OFFSET=0;
            NAME_2=load current symbol:CLOSE;
            NAME_2_OFFSET=1;
            OP=LT;
            NAME_3=create vol:NAME;
            NAME_3_OFFSET=0;
            COLOR=#FF0000";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update chart
$command = "STEP=update chart;
            COMMAND=CHART_UPDATE;
            CHART=create chart:NAME";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

