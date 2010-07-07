# this will print all the quote data to the console for the symbol provided

use IO::Socket;
my $sock = new IO::Socket::INET (
                                 PeerAddr => 'localhost',
                                 PeerPort => '5000',
                                 Proto => 'tcp',
                                );
die "Could not create socket: $!\n" unless $sock;

print $sock "GetQuotes,XNYS,MMM,D,19000101000000,20101231000000\n";
$rc = <$sock>;
close($sock);
print $rc;
