# this script will save quotes from a CSV file to the QuoteServer
# to test this script, you will have to supply an appropriate CSV file
# modify $fileName to the path of your CSV file

use IO::Socket;

# flush the buffers just in case
$|++;

# change this to suit your path ot the CSV file
$fileName = '/the/path/to/the/data/file';

# open the CSV file
open (MYFILE, $fileName);

# start with the base QuoteServer command
$command = "Quotes,Set,S,XNYS,GOOG,yyyyMMdd";

# loop through file one line at a time
while (<MYFILE>)
{
  # we just append all the records to the end of $command until done
  # we are also assuming that each line in the file is a valid record
  $command = join ",", $command, $_;
}

# we are done with the file
close (MYFILE);

# we wait to open the socket until we are ready to send the complete command
# because server will time out in 3 seconds, the script might not finish in time

my $sock = new IO::Socket::INET (
                                 PeerAddr => 'localhost', # QuoteServer default
                                 PeerPort => '5000', # QuoteServer default
                                 Proto => 'tcp',
                                );
die "Could not create socket: $!\n" unless $sock;

# send the complete command
print $sock "$command\n";

# get the return status, either "OK\n" or "ERROR\n"
$rc = <$sock>;

# done with the socket
close($sock);

# print the return status to the console
print $rc;



