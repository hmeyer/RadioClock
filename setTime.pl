
#!/usr/bin/perl

use strict;
use warnings;
use Time::HiRes qw(time sleep);
use Net::Telnet;
use DateTime;
use Data::Dumper;
use POSIX qw(strftime);
use Encode;
# sudo apt-get install libnet-telnet-perl libtime-hires-perl libdatetime-perl
$|=1;

my $host='192.168.179.101';
my $port='1000';
my $time_zone='Europe/Berlin';
my $video_path='/home/milan/radio/radiouhr/video/';

my $sleep =2;
my $max_events=3;
my $title_length=60;
my $message_length=100;
my $exit=0;
my $keep_open=1;

my $SEC   =  1;
my $MIN   = 60*$SEC;
my $HOUR  = 60*$MIN;
my $DAY   = 24*$HOUR;
my $SLEEP =  10*$SEC;

our $t = new Net::Telnet (
	Timeout => 10,
	ErrMode => \&reconnect,
	Prompt  => '/[\n\0x{0}]/',
	Ors => "\n",
	Rs => "\n",
	Ofs => "",
	Binmode => 1,
	Telnetmode => 0,
	Cmd_remove_mode => 0,
#	Input_log => 'out.log',
#	Output_log => 'out.log',
	
);

$SIG{INT} = sub {
	$exit=1;
	print"\n";
	#if($t->peerhost() eq ''){
	#	$t->cmd("q");
	#	sleep(1);
	#}
	closeConnection();
};

#get offset
my $epoch=time();
my $dt=DateTime->now();
$dt = DateTime->from_epoch(
	epoch => $epoch, 
	time_zone => $time_zone
);
my $offset=$dt->offset();

#openConnection();
#getSettings();
#while(1){
#	my $timeout=10;
#	my @videos=glob($video_path.'*.aviclock');
#	my $video=$videos[int(rand(@videos))];
#	my $startFrame=int(rand(1000));
#	startVideo($video, $timeout, $startFrame);
#	sleep 1;
#}
#closeConnection();

#playMovie($ARGV[0],$ARGV[1]||999999999,$ARGV[2]||0) if (defined $ARGV[0]);
main();


sub main{
	my $url='http://piradio.de/agenda/events.cgi?template=event_utc_time.txt&limit=20';

	my $c=0;
	my $plan='';
	while(1){
		
		openConnection();

		if($c==0){
			getSettings();
			#setBrightness(2);
			getBrightness();
		}

		#set scroll speed
		if (getScrollSpeed()!=60){
			setScrollSpeed(60);
		}

		#set time
		my $time=getTime();
		setTime() if((time()/60) % 5 ==0);

		#update agenda
		if($c % (10) == 0){
			#$plan=getTestPlan($time-$offset, $c) ;
			$plan=httpGet($url);
			#sleep 1;
			sendAgenda($plan);
		}

		#if(1==2){
		if($c %2 == 0){
			#start video
			if((!defined $ARGV[0]) || ($ARGV[0]eq'')){
				my $timeout=10;
				my @videos=glob($video_path.'*.aviclock');
				my $video=$videos[int(rand(@videos))];
				my $startFrame=int(rand(1000));
				startVideo($video, $timeout, $startFrame);
			}else{
				my $video=$ARGV[0];
				my $startFrame=0;
				my $timeout=10;
				startVideo($video, $timeout, $startFrame);
			}
		}else{
			#start message
			my $message=createMessage();
			$message=substr(encodeText($message), 0, $message_length);
			#my $timeout=int(length($message)/2);
			my $timeout=5+int(rand(10));
			sendMessage($timeout, $message);
		}

		closeConnection();

		sleep($SLEEP);
		$c++;
	}
}


sub startVideo{
	my $filename=shift;
	my $duration=shift;
	my $startFrame=shift||0;

	printInfo("send video request to play '$filename' for $duration seconds");
	telnetCmd("i$duration");

	printInfo("is video allowed...");
	my $result=telnetCmd("h");
	if($result=~/(\d)/){
		my $videoAllowed=$1;
		if($videoAllowed eq '1'){
			printInfo("get video status");
			telnetCmd("j");
			playMovie($filename, $duration, $startFrame);
		}else{
			printInfo("video not allowed by now");
		}
	}
}

sub setBrightness{
	my $brightness=shift;
	printInfo("set brightnes: ".$brightness);
	return telnetCmd("k".$brightness);
}

sub getBrightness{
	printInfo("get brightness...");
	return telnetCmd("l");
}

sub setScrollSpeed{
	my $speed=shift;
	printInfo("set scroll speed: ".$speed);
	return telnetCmd("c".$speed);
}

sub getScrollSpeed(){
	printInfo("get scroll speed...");
	return telnetCmd("d");
}


sub getTime(){
	printInfo("get time...");
	return telnetCmd("t");
}

sub getSettings{
	printInfo("get settings...");
	my $result=telnetCmd("e");
	if ($result=~/(\d+) (\d+) (\d+)/){
		$max_events=$1;
		$title_length=$2*8/5-1;
		$message_length=$3;
		printInfo("max events:$max_events, length:$title_length, message_length:$message_length");
		return $max_events;
	}
}

sub sendMessage{
	my $duration=shift;
	my $message=shift;
	printInfo("send message...");
	telnetCmd("m $duration $message");
	printInfo("get message...");
	telnetCmd("n");
	printInfo("sleep $duration seconds");
	sleep($duration);
}

sub createMessage{
	printInfo("create message");
	my $message=`fortune -s`;
	$message=(split(/\-\-/,$message))[0];
	$message=~s/[^a-zA-Z0-9öäüÖÄÜß\,\.\:\?\;]/ /g;
	$message=~s/ +/ /g;
	$message=~s/\s+$//g;
	my $i=0;
	while((length($message)>$message_length)&&($i<10)){
		my $message=`fortune -s`;
		$message=(split(/\-\-/,$message))[0];
		$message=~s/[^a-zA-Z0-9öäüÖÄÜß\,\.\:\?\;]/ /g;
		$message=~s/ +/ /g;
		$message=~s/\s+$//g;
		$i++;
	}
	#print $message."\n";
	return $message;
}

sub printDebugData{
	#return;
	my $data=shift;
	my $hexline=unpack("H*",$data);
	print $hexline."\n";
#	printInfo(substr($hexline,   0,130));
#	printInfo(substr($hexline, 130,130))if(length($hexline)>130);
#	printInfo(substr($hexline, 260,130))if(length($hexline)>260);

#	printInfo( substr($hexline,   0,2)
#                   ." ".substr($hexline,   2,64)." ".substr($hexline, 258,64));
#	printInfo("   ".substr($hexline,  66,64))if(length($hexline)>64);
#	printInfo("   ".substr($hexline, 130,64))if(length($hexline)>128);
#	printInfo("   ".substr($hexline, 194,64))if(length($hexline)>192);

}

sub setTime{
	printInfo("set time");
	my $epoch=time()+$offset;
	my $seconds=int($epoch);
	my $millis=$epoch-$seconds;

	while($millis>0.01){
		$epoch=time()+$offset;
		$seconds=int($epoch);
		$millis=$epoch-$seconds;
		#printInfo("set seconds:$seconds, milli:$millis");
	}
	sleep(0.2);
	printInfo("set seconds:$seconds, milli:$millis");
	telnetCmd("z".$epoch." ".int(1000*$millis));
	#build command
}

sub getTestPlan{
	my $time=shift;
	my $update=shift;
	
	printInfo("get test plan");
	my $plan='';
	my $delta=90;

	$time+=$delta;
	for my $nr(1..50){
		$plan.=$time;
		$plan.=" ";
		$time+=$delta;
		$plan.=$time;
		$plan.=" ".($update+$nr).".Sendung - hier kommt noch eine ganz lange Beschreibung hinter\n";
	}

	return $plan;

}

#91 ä
#92 ö
#93 ü
#123 ä
#124 Ö
#125 Ü
#126 ß

sub sendAgenda{
	my $plan=shift;
	printInfo("set plan");
	my $events=0;
	my $previous_end=undef;
	for my $line (split/\n/,$plan){
		my @fields=split(/\s+/,$line);
		my $start=shift @fields;
		my $end=shift @fields;

		my $title='';
		if($events<2){
			$title=join(' ',@fields);
			$title=' '.encodeTitle($title);
		}
		my $time=time();
		if($end>$time){
			#break if event starts 6000 seconds after end of previous
			last if ((defined $previous_end) and ($start-$previous_end>6000));
			my $result=telnetCmd("u".$events." ".($start+$offset)." ".($end+$offset).$title);
			$events++;
			last if ($events==$max_events);
			$previous_end=$end;
		}
	}
	
	for my $index (0..$events-1){
		my $time=time();
		printInfo("now:   ".(sprintf("%02f",$time+$offset))."         '".formatTime($time)."'");
		my $result=telnetCmd("s".$index);
		printInfo("now:  '".formatTime($time)."'");
		if($result=~/(\d) (\d+) (\d+) /){
			printInfo("start:'".formatTime($2-$offset)."', end:'".formatTime($3-$offset)."' duration:".($3-$2));
		}
	}
}

sub encodeTitle(){
	my $title=shift;
	$title=encodeText($title);
	#$title=(split(/ - /,$title))[0];
	$title=uc($title);
	$title=~s/[^A-Z]/ /g;
	$title=~s/\s+/ /g;
	$title=~s/ +/ /g;
	$title=~s/^\s+//;
	$title=~s/\s+$//;
	$title=substr($title,0,$title_length-1);
	return $title;
}

sub playMovie{
	my $filename=shift;
	my $duration=shift;
	my $start=shift;
	printInfo("play movie... '$filename' for $duration seconds");

	unless(defined $filename){
		printInfo("skip, no file given");
	}

	#openConnection();
	
	#clearscreen	
	#$t->print('a',("\0" x 128)."\n");
	#$t->waitfor('/\n/');
	#sleep(0.5);
	#$t->print('b',("\0" x 128)."\n");
	#$t->waitfor('/\n/');
	#sleep(0.5);

	open my $file, "<$filename";
	unless(defined $file){
		print_info("cannot read $filename");
		return;
	}
	binmode $file;

	my $stop=time()+$duration;

	my $frame=-1;
	my $data='';
	my $sleep=0.0;
	my $fps=5;
	my $show_a=1;
	my $show_b=0;
	my $mod=$fps*$sleep;
	$mod=1 if ($mod<1);

	$mod=1;
	for (0..$start){
		read($file, $data, 130);
		$frame++;
	}
	#printDebugData($data);

	my $time=time();
	printInfo("play video for ".int($stop-time())." seconds\n");
	while( read($file, $data, 130) ){
		print "$frame ";
		last if (time()>$stop);
		$frame++;

		if( ($frame % $mod) == 0 ){
			if($show_a and $show_b and ($frame % 2==1)){
				read($file, $data, 130);
				$frame++;
			}
			if ($show_a and $data=~/^a/){
				if ($data=~/\n/){
					$data=~s/\n/\x{00}/g;
					#$data=~s/\x{60}/\x{59}/g;
					#$data=~s/\x{40}/\x{3f}/g;
					#$data=~s/\0$/\n/g;
				}
				$data=substr($data,0,$message_length-1)."\n";
				my $duration=(time-$time);

				$time=time();
				printInfo("frame:$frame, length:".length($data)." layer:".substr($data,0,1)." duration:".$duration);# if($duration>0.3);
				printDebugData($data);
				$t->print($data);
				#print 
				$t->waitfor('/\n/');
				sleep($sleep);
			}

			if($show_b){
				read($file, $data, 130);
				$frame++;
			}
			if (($show_b) and ($data=~/^b/)){
				if ($data=~/\n/){
					$data=~s/\n/\x{00}/g;
					#$data=~s/\x{60}/\x{59}/g;
					#$data=~s/\x{40}/\x{3f}/g;
					#$data=~s/\0$/\n/g;
				}
				$data=substr($data,0,$message_length-1)."\n";
				my $duration=(time-$time);

				printInfo("frame:$frame, length:".length($data)." layer:".substr($data,0,1)." duration:".$duration);# if($duration>0.3);
				printDebugData($data);
				$t->print($data);
				#print 
				$t->waitfor('/\n/');
				sleep($sleep);
			}
		}
	}
	print "\n";
	#closeConnection();
}

sub encodeText(){
	my $title=shift;
	#$title=~s/Berliner Runde/äöüÄÖÜß/g;
	$title=~s/Ä/\x5B/g; # 91
	$title=~s/Ö/\x5C/g; # 92
	$title=~s/Ü/\x5D/g; # 93
	$title=~s/ä/\x7B/g; #123
	$title=~s/ö/\x7C/g; #124
	$title=~s/ü/\x7D/g; #125
	$title=~s/ß/\x7E/g; #126
	return $title;
}

sub telnetCmd{
	my $cmd=shift;
	$cmd=substr($cmd,0,130);
	printInfo("put '$cmd'");
	#$cmd=decode('utf8',$cmd);

	#my $result=join('',$t->cmd($cmd));
	$t->print($cmd);
	#sleep(1);
	my $result=join('',($t->waitfor('/\n/')));
	chomp $result;
	printInfo("get '$result'");
	return $result;
}

sub reconnect{
	printInfo("close...");
	$t->close();
	exit if($exit==1);

	if($keep_open==1){
		printInfo("timeout. reconnect...");
		$t->open(
			Host => $host,
			Port => $port
		);
	}
}

sub openConnection{
	printInfo("connect...");
	$keep_open=1;
	$t->open(
		Host => $host,
		Port => $port
	);
}
sub closeConnection{
	printInfo("close...");
	telnetCmd('q');
	$keep_open=0;
	$t->close();
	if($exit==1){
		printInfo("exit");
		exit;
	}
}

sub httpGet{
	my $url		= shift;
	my $gateway	= shift;

	my $cmd='wget -qO-';
	$cmd.=' --bind-address '.$gateway if (defined $gateway);
	$cmd.=' --tries 1 ';
	$cmd.=' --timeout 5 ';
	$cmd.=' '.$url;
	$cmd.=' 2>&1';
	printInfo("execute  '".$cmd."'");

	my $result=`$cmd`;
	#printInfo($result);
	return $result;
}

sub formatTime{
	my $time=shift;
	return strftime("%Y-%m-%d %H:%M:%S", localtime($time));
}

sub printInfo{
	print formatTime(time())." ".sprintf("%02f",time())." -- INFO -- ".$_[0]."\n";
}


