# stops the server from updating yahooquotes if it is running

use IO::Socket;
my $sock = new IO::Socket::INET (
                                 PeerAddr => 'localhost',
                                 PeerPort => '5000',
                                 Proto => 'tcp',
                                );
die "Could not create socket: $!\n" unless $sock;

print $sock "YahooQuotes,Stop\n";
$rc = <$sock>;
close($sock);
