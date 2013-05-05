#!/usr/bin/perl

use Time::HiRes qw(time sleep);
use Net::Telnet;
use DateTime;
use Data::Dumper;
# sudo apt-get install libnet-telnet-perl libtime-hires-perl libdatetime-perl

my $host='192.168.179.101';
my $port='1000';
my $time_zone='Europe/Berlin';
my $sleep =60;

$t = new Net::Telnet (
	Timeout => 10,
	ErrMode => \&reconnect
);

reconnect();
my $dt=DateTime->now();

while(1){
	my $epoch=time();
	$dt = DateTime->from_epoch( epoch => $epoch, time_zone => $time_zone);
	print "INFO: "
	.$dt->ymd()
	." ".$dt->hms().".".substr($dt->nanosecond(),0,3)
	."\tepoch:".$epoch
	#." ".$dt->epoch()
	."\n";

	#add current timezone
	$epoch+=$dt->offset();	
	
	#lower and higher 32bit
	#print "INFO: ".(int($epoch*1000)/0xFFFFFFFF)." ".(int($epoch*1000)%0xFFFFFFFF)."\n";

	#build command
	my $cmd="t".int($epoch*1000);
	print "< $cmd\n";

	#output result
	$result=join("\n",$t->cmd($cmd));
	$result=~s/\n+$//;
	print "> $result\n";

	sleep $sleep;
}

sub reconnect{
	print "INFO: close...\n";
	$t->close();
	sleep(1);

	print "INFO: connect...\n";
	$t->open(
		Host => $host,
		Port => $port
	);
}

