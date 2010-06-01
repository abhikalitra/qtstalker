"""
    Example script which loads historical EOD data from Yahoo Finance.  
    
    To specify which symbols to load data for, and the starting month/year:
    create a CSV file in your home directory (i.e. value of $HOME) called 
    "qts_stocks.csv".  The first line of this file will be the staring date
    for the historical period and is the zero-indexed month, followed by comma,
    then the full four digit year.  e.g. "04,2010"  would be May 2010.
    the following lines are pairs of "exchange code, symbol"; where 
    "exchange code" must be one of the codes from the first field of "exchanges.csv",
    which is located in "/usr/local/share/qtstalker/db/exchanges.csv" on UNIX 
    platforms.  Here is an example "$HOME/qts_stocks.csv":
    
    04, 2010
    "XASE","A"
    "XASE","T"
    "XASE","VZ"
    "XNAS","MSFT"
    "XNAS","ORCL"

    When the data is loaded, a new group is created called "DemoGroup"
    (currenly hard-coded) to which the symbols will be associated.

    Fully tested on MacOs-10.5.8 on Python 2.4
    Partially tested on WindowsXP on ActiveState Python 2.5
    
    Author: Chris Wolf
"""
import sys, os, urllib, csv

# uncomment and replace with appropriate HTTP proxy settings, if you're behind
# a corporate firewall which requires a proxy to hit outside sites
#proxies = {'http' : 'http://127.0.0.1:8080'}
proxies = {}

# global definitions
qtSriptIO = None
log = None
isWin = False
homeDir = None

class AutoFlushWriter:
    """ Currently only used for debug logger """
    def __init__(self, stream):
        self.stream = stream
    def write(self, data):
        self.stream.write(data)
        self.stream.flush()
    def __getattr__(self, attr):
        return getattr(self.stream, attr)

class QtScriptIO:
    """ this object encapsulates writing a QTStalker script command to stdout, then 
    reading the command result status from stdin; if the status is non-zero, then the
    offending command is written to stderr and the script exits.
    """
    def __init__(self, stream):
        self.stream = stream
    def read_stdin(self):
        line = sys.stdin.readline();
        status = line.rstrip('\n') 
        return int(status);
    def write(self, data):
        """ for some unknown reason, extra lines with a single space are comming in, so we 
        filter them out here."""
        cleaned = data.strip()
        if len(cleaned) < 1:
            return
        self.stream.write(cleaned)
        self.stream.flush()
        if log != None:
            print >> log, ("|>%s<|" % data)
        # comment out the following four lines for test runs from the command line:
        status = self.read_stdin()
        if status != 0:
            print stderr ("Error in script: %s" % data)
            sys.exit(status)
    def __getattr__(self, attr):
        return getattr(self.stream, attr)

def itoa(i, base=10):
    """ Integer-to-ASCII """
    return str(int(str(i), base))

def get_exchange_and_company_name(symbol):
    """ This is used to get the company name given a ticker symbol.  The exchange name
    returned, is the Yahoo Finance exchange name, and usually has no bearing or relation
    to the exchange code used by QtStalker, so this exchange name is not used.
    """
    params = urllib.urlencode({'s': symbol, 'f': 'xsn'})

    httpClient = urllib.FancyURLopener(proxies)
    u = httpClient.open("http://download.finance.yahoo.com/d/ndata.csv?%s" \
        % params)
    csvstr = u.readline()
    #print ("|>%s<|\n" % csvstr);
    u.close()
    firstrow = []
    firstrow.append(csvstr)
    reader = csv.reader(firstrow)
    return reader.next()

def get_historical_prices(symbol, since_month, since_year):
    """ Retrieves EOD pricing data in the format of:
    "date, open, high, low, close, volume", as required by the QTStalker SET_QUOTE
    command.  The time will be hard-coded to "4PM" in the load_data() function.
    """
    params = urllib.urlencode({'s': symbol, \
                               'a':  itoa(since_month), \
                               'b':  '1',  \
                               'c':  itoa(since_year)})

    httpClient = urllib.FancyURLopener(proxies)
    u = httpClient.open("http://ichart.finance.yahoo.com/table.csv?%s" % params)

    #print u.read()
    u.readline() # first row are field name captions; discard
    lines = u.readlines()
    u.close()
    return lines

def load_data():
    """ for each ticker symbol in "~/qts_stocks.csv", fetch the company name from Yahoo,
    then fetch the EOD price history for the period.  Add the ticker symbol to a hardcoded
    group named "DemoGroup".
    """
    stock_sym_file = homeDir + 'qts_stocks.csv'
    reader = csv.reader(open(stock_sym_file))
    start_date_row = reader.next()

    for exch_sym_row in reader:
        exch, sym  = exch_sym_row
        lines = get_historical_prices(sym, start_date_row[0], start_date_row[1])
        yahooexch_sym_company = get_exchange_and_company_name(sym)
        company_name = yahooexch_sym_company[2]

        print >> qtScriptIO, ("QUOTE,Stock,SET_NAME,%s,%s,%s" % (exch, sym, company_name)),
        print >> qtScriptIO, ("GROUP,ADD,DemoGroup,%s,%s" % (exch, sym)),
        for line in lines:
            row = line.rstrip('\n').split(',')
            print >> qtScriptIO, ("QUOTE,Stock,SET_QUOTE,%s,%s,yyyy-MM-dd hap,%s 4pm,%s,%s,%s,%s,%s" % (exch, sym, row[0], row[1], row[2], row[3], row[4], row[5])),

        #print ("\nSymbol: %s:%s" % (exch,sym))
        #for line in lines:
        #    print ("|>%s<|" % line.rstrip())

    print >> qtScriptIO, ("QUOTE,Stock,SAVE_QUOTES"),

if __name__ == "__main__":
    try:
        import msvcrt
        isWin = True
        homeDir = os.environ['USERPROFILE'] + "\\"
    except ImportError:
        homeDir = os.environ['HOME'] + "/"
        isWin = False

    #log = AutoFlushWriter(open("/tmp/script.log", "w"))
    qtScriptIO = QtScriptIO(sys.stdout)
    load_data()
