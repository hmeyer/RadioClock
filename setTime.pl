#!/usr/bin/perl

use strict;
use warnings;
use Time::HiRes qw(time sleep);
use Net::Telnet;
use DateTime;
use Data::Dumper;
use POSIX qw(strftime);
use Getopt::Long;

#required libs
# sudo apt-get install libnet-telnet-perl libtime-hires-perl libdatetime-perl
$|=1;

my $SEC   =  1;
my $MIN   = 60*$SEC;
my $HOUR  = 60*$MIN;
my $DAY   = 24*$HOUR;
my $SLEEP = 10*$SEC;
my $targetFrameTime=1.0/25;

my $time_shift=0; #4*$DAY-8*$HOUR;
#radio clock
my $host='192.168.2.87';
my $port='1000';

#local paths
my $video_speed=1;
my $video_path='/home/radioadmin/radio/RadioClock/video/';
my $log_file='/var/log/radioclock/radioclock.log';
my $message_file='/tmp/radioMessage.txt';

my $url='http://piradio.de/agenda/events.cgi?template=event_utc_time.txt&limit=10';
#$url.='&from_date=2013-12-18&archive=all';
my $time_zone='Europe/Berlin';

#configuration
my $noMessageAfterHour=19;
my $long_videos_abort_hour=19;
my $red_video_channel_only=0;
my $video_default_length=60;

#parameters
my $debug=0;
my $video_file_pattern='';
GetOptions(
	"video=s" => \$video_file_pattern,
	"debug=s" => \$debug
);

#update periods
my $setTimeEvery     = 10*$MIN;
my $sendPlanEvery    = 10*$MIN;
my $showVideoEvery   =  5*$MIN;
my $showMessageEvery =  3*$MIN;

my $lastSetTime=0;
my $lastSendPlan=0;
my $lastShowVideo=0;
my $lastShowMessage=0;

#radio clock presets (current settings wil be read from radioclock)
my $max_events=3;
my $title_length=60;
my $message_length=100;
my $max_brightness=1;
my $scrollSpeed=1;
my $brightness=1;

#internal states
my $exit=0;
my $keep_open=1;
my $stop_video=0;

openLogFile();

#reopen log files on signal HUP
$SIG{HUP} = &reopenLogFile;

our $t = getTelnet();

#register 
$SIG{INT} = sub {
	$exit=1;
	printInfo(0,"external close");
	closeConnection();
};

#get offset
my $epoch=now();
my $dt = DateTime->from_epoch(
	epoch 		=> $epoch, 
	time_zone 	=> $time_zone
);
printInfo($dt->ymd()." ".$dt->hms());
my $offset=$dt->offset();
my $cycles=0;
my $ticks=0;
my $prevCycleTime=0;
my $prevCycles=0;
my $prevTicks=0;

main();

sub main{
	my $c=0;
	my $plan='';
	$plan=httpGet($url);
	while(1){
		reopenLogFile() if(tell(STDERR) != -1);
		my $eventFound=undef;
		print"\n";
		openConnection();

		#get settings and statistics
		getSystemStart();
		while(! defined getSettings()){
			sleep 5;
			getSettings();
			setScrollSpeed(6) if ($scrollSpeed!=6);
			sleep 1;
		}

		#set time
		my $time=getTime();
		if ($time-$lastSetTime>$setTimeEvery){
			setTime();
			sleep(1);
			$lastSetTime=$time;
		}

		#update plan
		if ($time-$lastSendPlan>$sendPlanEvery){
			$plan=httpGet($url);
			#$plan=getTestPlan($time-$offset, $c) if($c%3==0);
			sendPlan($plan);
			sleep(1);
			$lastSendPlan=$time;
		}

		#check for message before any action
		unless(defined $eventFound){
			my $message=showAnyIncomingMessage();
			$eventFound=1 if(defined $message);
		}

		#play video
		unless(defined $eventFound){
			if($time-$lastShowVideo>$showVideoEvery){
				my @videos=glob($video_path.'*.aviclock');
				if($video_file_pattern ne''){
					my @videos2=();
					for my $video(@videos){
						push @videos2,$video if($video=~/$video_file_pattern/);
					}
					@videos=@videos2;
				}
				if (@videos>0){
					my $video=$videos[rand(@videos)];
					startVideo($video);
					$eventFound=1;
				}
				print Dumper(\@videos);
				$time=getTime();
				$lastShowVideo=$time;
			}
		}

		#show message
		unless(defined $eventFound){
			if($time-$lastShowMessage>$showMessageEvery){
				if(getHour(now())<$noMessageAfterHour){
					my $message=createMessage();			
					if($c%2==0){
						showMessage($message);
						$eventFound=1;
					}else{
						scrollMessage($message);
						$eventFound=1;
					}
				}
				$time=getTime();
				$lastShowMessage=$time;
			}
		}

		showAnyIncomingMessage();
		
		closeConnection();
#		printInfo("sleep $SLEEP");
		if($SLEEP>15){
			my $sleep=$SLEEP;
			while($sleep>15){
				#printInfo("sleep again ".(15-(now()%15)));
				$sleep-=5;
				sleep 5;
				
				my $message=readMessage();
				if (defined $message){
					openConnection();
					showMessage($message);
					closeConnection();
				}
			}
		}
		sleep(15-(now()%15));
		$c++;
	}
}

sub showAnyIncomingMessage{
	my $message=readMessage();
	return undef unless (defined $message);
	showMessage($message);
	my $time=getTime();
	$lastShowMessage=$time;
	return 1;	
}

sub keepAlive{
	telnetCmd("y");
}

sub startVideo{
	my $filename=shift;
	my $timeout=shift||$video_default_length;
	my $startFrame=shift||0;

	$stop_video=0;
	my $size=int((stat $filename)[7]/130);
	my $duration=$size/10;
	#$startFrame=int(rand(1000));
	$timeout=int(($size-$startFrame)/10) if(getHour(now())<$long_videos_abort_hour);
	$timeout++ if($duration>$timeout);

	printInfo("send request to play '$filename' (duration: $duration seconds) for $timeout seconds");
	telnetCmd("i $timeout");

	printInfo("check, if video request is acknowledged");
	my $result=telnetCmd("h");
	if($result=~/(\d)/){
		my $videoAllowed=$1;
		if($videoAllowed eq '1'){
			if($debug>1){
				printInfo("get video status");
				printInfo(telnetCmd("j"));
			}
			if($filename=~/piradio/){
				playMovie($filename, $timeout, $startFrame, 1);
			}else{
				playMovie($filename, $timeout, $startFrame, $red_video_channel_only);
			}
		}else{
			printInfo("video not allowed by now. please try again later");
		}
	}
	printInfo("video finished");
}

sub setBrightness{
	my $brightness=shift;
	printInfo("set brightnes: ".$brightness);
	if($brightness<$max_brightness){
		return telnetCmd("k ".$brightness);
	}else{
		return "invalid value!\n";
	}
}

sub setScrollSpeed{
	my $speed=shift;
	printInfo("set scroll speed: ".$speed);
	return telnetCmd("c ".$speed);
}

sub getScrollSpeed(){
	return $scrollSpeed;
}

sub getTime(){
	printInfo("get time...");
	return telnetCmd("t");
}

sub getHour{
	my $unixtime=shift;
	return int($unixtime % $DAY / $HOUR);
}

sub getSystemStart(){
	printInfo("get SystemStart..");
	my $result=telnetCmd("f");
	printInfo($result);
	my $time=now();
	my $getSystemStartTime=now();
	if($result=~/(\d+) (\d+)( (\d+))?( (\d+))?/){
		my $systemStart=$1;
		my $reconnects=$2;
		$cycles=$4||0;
		$ticks=$4||0;
		$prevCycleTime=$time-1 if($time==$prevCycleTime);
		my $cps=(($cycles-$prevCycles)/($time-$prevCycleTime));
		$cps=0 if($cps<0);
		$cps=sprintf("%.3f",$cps);
		my $tps=(($ticks-$prevTicks)/($time-$prevCycleTime));
		$tps=0 if($tps<0);
		$tps=sprintf("%.3f",$tps);
		my $dt=DateTime->from_epoch( epoch => $systemStart);
		my $duration=$time+$offset-$systemStart;
		$duration=sprintf("%02d:%02d:%02d", int($duration/$HOUR), int(($duration/$MIN)%60), int($duration%60));
		printInfo("start at '".$dt->ymd()." ".$dt->hms()."' recon:$reconnects online: $duration cycles:$cycles cps:$cps ticks:$ticks tps:$tps");
	}
	$prevCycleTime=$time;
	$prevCycles=$cycles;
	$prevTicks=$ticks;
}

sub getSettings{
	printInfo("get settings...");
	my $result=telnetCmd("e");
	if ($result=~/(\d+) (\d+) (\d+) (\d+) (\d+) (\d+)/){
		$max_events=$1;
		$title_length=$2*8/5-1;
		$message_length=$3;
		$max_brightness=$4;
		$scrollSpeed=$5;
		$brightness=$6;
		printInfo("max events:$max_events, length:$title_length, message_length:$message_length, max_brightness:$max_brightness"
			." scrollSpeed:$scrollSpeed brightness:$brightness"
		);
		return $max_events;
	}
	return undef;
}

sub scrollMessage{
	my $message=shift;
	my $duration=shift;

	$message=encodeText($message);
	$message=substr($message, 0, $message_length-1);
	$duration=int(length($message)*0.4);

	$duration=1 if($duration<1);
	printInfo("scroll message... '$message'");
	telnetCmd("m $duration 1 $message");
	printInfo("get message...");
	telnetCmd("n");
	printInfo("sleep $duration seconds");
	sleep($duration);
	keepAlive();
}

sub showMessage{
	my $message=shift;
	my $duration=shift;

	$message=encodeText($message);
	$message=substr($message, 0, $message_length);
	$duration=int(length($message)*0.4);

	my @words=split(/ +/,$message);
	my $i=0;
	my $words='';
	while ($i<=@words){
		while( ($i<@words) && (length($words)+length($words[$i])<15) ){#16
			$words.=$words[$i].' ';
			$i++;
		}
		$words=encodeTitle($words);
		$words=substr($words,0,16).chr(0);
		last if ($words eq '');
		my $sleep=int($duration*length($words)/(length($message)+1));
		printInfo("show message... '$words'");
		$sleep=2 if($sleep<=1);

		telnetCmd("m $sleep 0 $words");
		printInfo(3,"sleep $sleep seconds");
		sleep($sleep);
		$words=$words[$i].' ' if($i<@words);
		$i++;
	}
	sleep(5);
	keepAlive();
}

sub createMessage{
	#printInfo("create message");
	my $message=`fortune -s`;
	$message=(split(/\-\-/,$message))[0];
	$message=~s/[^a-zA-Z0-9öäüÖÄÜß\,\.\:\?\;]/ /g;
	$message=~s/ +/ /g;
	$message=~s/\s+$//g;
	return $message;
}

sub videoDataInfo{
	my $data=shift;
	my $hexline=unpack("H*",$data);
	return $hexline;
}

sub setTime{
	printInfo("set time");
	my $epoch=now()+$offset;
	my $seconds=int($epoch);
	my $millis=$epoch-$seconds;

	while($millis>0.01){
		$epoch=now()+$offset;
		$seconds=int($epoch);
		$millis=$epoch-$seconds;
		#printInfo("set seconds:$seconds, milli:$millis");
	}
	sleep(0.2);
	printInfo("set time:'".formatTime($epoch)."' seconds:$seconds (milli:".int(1000*$millis).")");
	telnetCmd("z ".$epoch);
}

sub getTestPlan{
	my $time=shift;
	my $update=shift;
	
	printInfo("get test plan");
	my $plan='';
	my $delta=int(10*rand())+10;

	$time+=$delta;
	for my $nr(1..4){
		$delta=int(10*rand())+10;
		$plan.=$time;
		$plan.=" ";
		$time+=$delta;
		$plan.=$time;
		$plan.=" ".($update+$nr).".Sendung - hier kommt noch eine ganz lange Beschreibung hinter\n";
	}

	return $plan;

}

sub sendPlan{
	my $plan=shift;
	printInfo("set plan");
	my $events=0;
	my $previous_end=undef;
	for my $line (split/\n/,$plan){
		my @fields=split(/\s+/,$line);
		my $start = shift @fields;
		my $end   = shift @fields;
	
		my $title='';
		if($events<2){
			$title=join(' ',@fields);
			$title=encodeTitle($title).chr(0);
		}
		my $time=now();
		#printInfo("time:$time start:$start end:$end");
		if($end>$time){
			#break if event starts 6000 seconds after end of previous
			last if ((defined $previous_end) and ($start-$previous_end>20000));
			printInfo(2,"start:'".formatTime($start+$offset)."' end:'".formatTime($end+$offset)."' title:'".$title."'");
			my $result=telnetCmd("u ".$events." ".($start+$offset)." ".($end+$offset)." ".$title);
			$events++;
			last if ($events==$max_events);
			$previous_end=$end;
		}
	}
}

sub encodeTitle(){
	my $title=shift;
	$title=~s/Berliner Runde - //g;
	$title=encodeText($title);
	$title=~s/\s+/ /g;
	$title=~s/^\s+//;
	$title=~s/\s+$//;
	$title=substr($title,0,$title_length-1);
	return $title;
}

sub playMovie{
	my $filename=shift;
	my $duration=shift;
	my $start=shift;
	my $red_video_channel_only=shift;

	my $info="play movie... '$filename' for $duration seconds";
	$info.=" start with frame $start" if($start>0);
	printInfo($info);
	printInfo("skip, no file given") unless(defined $filename);

	open my $file, "<$filename";
	unless(defined $file){
		print_info("cannot read $filename");
		return;
	}
	binmode $file;

	my $frame=0;
	my $data='';
	for (0..$start){
		read($file, $data, 130);
		$frame++;
	}

	my $time=now();
	my $stop=$time+$duration;

	my $old_frame=$frame;
	my $old_time=$time-8;

	printInfo("play video for ".int($stop+1-now())." seconds, speed: $video_speed\n");
	while( read($file, $data, 130) ){
		$time=now();
		last if ($time>$stop);
		last if ($stop_video==1);

		if($debug>=0){
			if($frame%100==0){
				my $fps=(($frame-$old_frame)/($time-$old_time));
				$fps=sprintf("%d",$fps);
				printInfo("video frame:$frame rate:$fps fps" );
				#check if message has been come in
				my $message=readMessage();
				$old_frame=$frame;
				$old_time=$time;
				if(defined $message){
					disableVideo();
					showMessage($message);
					enableVideo();
				}
			}
			print "." if(($debug>0)&&($frame%10==0));
		}

		if( ($frame % $video_speed) == 0 ){
			if ($data=~/^a/){
				$data=~s/\n/\x{00}/g if ($data=~/\n/);

				if($red_video_channel_only){
					my $data2='a';
					for (my $i=1;$i<length($data);$i+=16){
						$data2.=substr($data,$i,8).(chr(0) x 8);
					}
					$data=$data2;
				}

				$data=substr($data,0,129)."\n";

				printInfo(3, "frame:$frame, length:".length($data)." layer:".substr($data,0,1));#." duration:".$duration);
				printInfo(6, videoDataInfo($data));

				#send frame
				my $frameTime=now();
				$t->print($data);
				my $result=join('',($t->waitfor('/\n/')));
				keepAlive();
				my $frameDuration=now()-$frameTime;
				#sleep(0.01);
				printInfo(3,"duration:".$frameDuration." target:$targetFrameTime");
				if($frameDuration<$targetFrameTime){
					printInfo(3,"sleep:".$frameDuration);
					sleep($targetFrameTime-$frameDuration);
				}
				printInfo(3,$result);

				last if ((defined $result)&&($result=~/BRK/));
				#sleep(1);
			}
		}
		$frame++;
	}
	print "\n";
	disableVideo();
}

sub enableVideo{
	printInfo("enable video");
	telnetCmd("o");
}

sub disableVideo{
	printInfo("disable video");
	telnetCmd("p");
}

sub encodeText(){
	my $title=shift;
	$title=~s/Ä/\[/g; # 91
	$title=~s/Ö/\\/g; # 92
	$title=~s/Ü/\]/g; # 93
	$title=~s/ä/\{/g; #123
	$title=~s/ö/\|/g; #124
	$title=~s/ü/\}/g; #125
	$title=~s/ß/\~/g; #126
	$title=~s/\s+/ /g;
	$title=~s/\s+$//g;
	return $title;
}

sub getTelnet{
	return new Net::Telnet (
		Timeout => 10,
		ErrMode => \&reconnect,
		Prompt  => '/\n/',
		Ors => "\n",
		Rs => "\n",
		Ofs => "",
		Binmode => 1,
		Telnetmode => 0,
		Cmd_remove_mode => 0,
	#	Input_log => 'out.log',
	#	Output_log => 'out.log',
	
	);
}

sub telnetCmd{
	my $cmd=shift;
	my $data=shift||undef;
	unless(defined $data){
		$cmd=substr($cmd,0,130);
		printInfo(2, "put '$cmd'");
	}
	$t->print($cmd);
	my $result=join('',($t->waitfor('/\n/')));
	chomp $result;
	printInfo(2, "get '$result'");
	return $result;
}

sub reconnect{
	printInfo("got disconnected?! close connection.");
	$t->close();
	exit if($exit==1);
	sleep(1);
	$stop_video=1;
	if($keep_open==1){
		printInfo("connection timeout. reconnect...");
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
	printInfo("close connection...");
	telnetCmd('q');
	$keep_open=0;
	$t->close();
	if($exit==1){
		printInfo("exit");
		exit;
	}
}

sub readMessage{
	return undef unless (-e $message_file);
	my $message='';
	open my $file, "<:crlf", $message_file;
	while(<$file>){
		$message.=$_;
	}
	close $file;
	unlink $message_file;
	$message=~s/\s+/ /g;
	$message=~s/^\s//g;
	$message=~s/\s$//g;
	printInfo("readMessage :".$message);
	return $message;
}

sub httpGet{
	my $url		= shift;
	my $gateway	= shift;

	my $cmd='wget -qO-';
	$cmd.=' --bind-address '.$gateway if (defined $gateway);
	$cmd.=' --tries 1 ';
	$cmd.=' --timeout 5 ';
	$cmd.=' '."'$url'";
	$cmd.=' 2>&1';
	printInfo("execute  '".$cmd."'");

	my $result=`$cmd`;
	printInfo(3,$result);
	return $result;
}

sub formatTime{
	my $time=shift;
	return strftime("%Y-%m-%d %H:%M:%S", localtime($time));
}

sub printInfo{
	my $level=0;
	$level=shift @_ if(@_==2);
	print(formatTime(now())." -- INFO   -- ".$_[0]."\n") if($debug>=$level);
}

sub printServerInfo{
	my $level=0;
	$level=shift @_ if(@_==2);
	print("\n".formatTime(now())." -- SERVER -- ".$_[0]."\n") if($debug>=$level);
}

sub openLogFile{
	my @log_dir=split/\//,$log_file;
	pop @log_dir;
	my $log_dir=join('/',@log_dir);
	`chmod -R 777 $log_dir/*`;
	if(-e $log_file){
		open STDOUT, '>>:utf8', $log_file || die ("cannot write to '$log_file'");
		open STDERR, '>>:utf8', $log_file || die ("cannot write to '$log_file'");
	}else{
		open STDOUT, '>:utf8', $log_file || die ("cannot write to '$log_file'");
		open STDERR, '>:utf8', $log_file || die ("cannot write to '$log_file'");
	}
}

sub reopenLogFile{
	close STDOUT;
	close STDERR;
	openLogFile();
};

sub execute{
	my $cmd=shift;
	print "execute:$cmd\n";
	print `$cmd`;
}

sub now{
	return time()-$time_shift;
}

