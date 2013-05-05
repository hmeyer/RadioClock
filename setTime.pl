#!/usr/bin/perl

use Time::HiRes;
use Net::Telnet ();

# sudo apt-get install libnet-telnet-perl libtime-hires-perl

my $host='192.168.179.101';
my $port='1000';

$t = new Net::Telnet (
	Timeout => 10,
	Prompt => '/bash\$ $/'
);

$t->open(
	Host => $host,
	Port => $port
);

while(1){
	my $time=int(Time::HiRes::gettimeofday()*1000);
	print $time."\n";
	my @result=$t->cmd($time);
	print @result."\n";
}
