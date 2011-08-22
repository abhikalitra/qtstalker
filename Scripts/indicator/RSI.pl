# qtstalker VOL indicator

###################################################################

$|++;

# create the chart
$command = "STEP=create chart;
            COMMAND=CHART;
            NAME=RSI;
            DATE=false;
            LOG=false;
            ROW=1;
            COL=1";
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

# create RSI
$command = "STEP=create RSI;
            COMMAND=RSI;
            NAME=rsi;
            INPUT=load current symbol:CLOSE;
            PERIOD=10";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot RSI
$command = "STEP=plot RSI;
            COMMAND=PLOT_LINE;
            CHART=create chart:NAME;
            DATE=load current symbol:DATE;
            INPUT=create RSI:NAME;
            NAME=RSI;
            COLOR=#ffff00;
            Z=0;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create buy zone
$command = "STEP=buy zone;
            COMMAND=CHART_OBJECT_HLINE;
            CHART=create chart:NAME;
            PRICE=30;
            COLOR=#00ff00;
            Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create sell zone
$command = "STEP=sell zone;
            COMMAND=CHART_OBJECT_HLINE;
            CHART=create chart:NAME;
            PRICE=70;
            COLOR=#ff0000;
            Z=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update chart
$command = "STEP=update chart;
            COMMAND=CHART_UPDATE;
            CHART=create chart:NAME";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
