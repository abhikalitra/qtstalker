# qtstalker OHLC indicator

###################################################################

$|++;

# create the chart
$command = "STEP=create chart;
            COMMAND=CHART;
            NAME=OHLC;
            DATE=true;
            LOG=false;
            ROW=0;
            COL=0";
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

# plot OHLC bars
$command = "STEP=create and plot ohlc;
            COMMAND=PLOT_OHLC;
            CHART=create chart:NAME;
            NAME=ohlc;
            DATE=load current symbol:DATE;
            OPEN=load current symbol:OPEN;
            HIGH=load current symbol:HIGH;
            LOW=load current symbol:LOW;
            CLOSE=load current symbol:CLOSE;
            COLOR=#0000ff;
            Z=0;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color up bars
$command = "STEP=color up ohlc;
            COMMAND=COLOR;
            NAME=load current symbol:CLOSE;
            NAME_OFFSET=0;
            NAME_2=load current symbol:CLOSE;
            NAME_2_OFFSET=1;
            OP=GT;
            NAME_3=create and plot ohlc:NAME;
            NAME_3_OFFSET=0;
            COLOR=#00ff00";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# color down bars
$command = "STEP=color down ohlc;
            COMMAND=COLOR;
            NAME=load current symbol:CLOSE;
            NAME_OFFSET=0;
            NAME_2=load current symbol:CLOSE;
            NAME_2_OFFSET=1;
            OP=LT;
            NAME_3=create and plot ohlc:NAME;
            NAME_3_OFFSET=0;
            COLOR=#ff0000";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create MA1
$command = "STEP=create MA1;
            COMMAND=MA;
            NAME=ma1;
            INPUT=load current symbol:CLOSE;
            PERIOD=20;
            METHOD=EMA";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create MA2
$command = "STEP=create MA2;
            COMMAND=MA;
            NAME=ma2;
            INPUT=load current symbol:CLOSE;
            PERIOD=50;
            METHOD=EMA";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# create MA3
$command = "STEP=create MA3;
            COMMAND=MA;
            NAME=ma3;
            INPUT=load current symbol:CLOSE;
            PERIOD=200;
            METHOD=EMA";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot MA1
$command = "STEP=plot MA1;
            COMMAND=PLOT_LINE;
            CHART=create chart:NAME;
            DATE=load current symbol:DATE;
            INPUT=create MA1:NAME;
            NAME=MA20;
            COLOR=#ff0000;
            Z=1;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot MA2
$command = "STEP=plot MA2;
            COMMAND=PLOT_LINE;
            CHART=create chart:NAME;
            DATE=load current symbol:DATE;
            INPUT=create MA2:NAME;
            NAME=MA50;
            COLOR=#ffff00;
            Z=1;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# plot MA3
$command = "STEP=plot MA3;
            COMMAND=PLOT_LINE;
            CHART=create chart:NAME;
            DATE=load current symbol:DATE;
            INPUT=create MA3:NAME;
            NAME=MA200;
            COLOR=#0000ff;
            Z=1;
            PEN=1";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }

# update chart
$command = "STEP=update chart;
            COMMAND=CHART_UPDATE;
            CHART=create chart:NAME";
print STDOUT $command;
$rc = <STDIN>; chomp($rc); if ($rc eq "ERROR") {print STDERR $command; exit; }
