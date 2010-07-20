# request search for all exchanges and all symbols in the quote database and prints the results to the console

use IO::Socket;
my $sock = new IO::Socket::INET (
                                 PeerAddr => 'localhost',
                                 PeerPort => '5000',
                                 Proto => 'tcp',
                                );
die "Could not create socket: $!\n" unless $sock;

#print $sock "Search,S,XNYS,MMM\n";
print $sock "Search,S,*,*\n";
$rc = <$sock>;
close($sock);
print $rc;
