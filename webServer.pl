#!/usr/bin/perl

use strict;
use warnings;

use Time::HiRes qw(time sleep);
use Net::Telnet;
use DateTime;
use Data::Dumper;
use POSIX qw(strftime);

use Encode;
use HTTP::Daemon;
use HTTP::Status;
use CGI;
use URI::Escape;
#$|=1;

my $debug=0;

my $port=8080;
my $message_file='/tmp/radioMessage.txt';
our $log_file='/var/log/radioclock/radioclock.log';
openLogFile();

#reopen log files on signal HUP
$SIG{HUP} = sub {
	close STDOUT;
	close STDERR;
	openLogFile();
};

while (1){
	#eval{
		setupHttpServer();
	#}
}

sub setupHttpServer{
	my $daemon=undef;
	while(!$daemon){
		$daemon = HTTP::Daemon->new(
			LocalPort => $port,
		);
		printServerInfo("HTTP server try to bind to port $port");
		sleep 10;
	}
	printServerInfo("now you can access the server at: ".$daemon->url);
	while (($daemon) && (my $c = $daemon->accept())) {
#		eval{
			my $request = $c->get_request();
			if ($request->method eq 'GET') {
				#print Dumper($request);
				my $params=parseUri($request->uri());
				if( (defined $params->{message}) && ($params->{message}ne'') ){
					$params->{message}=uri_unescape($params->{message});
					printServerInfo("send message: '$params->{message}'");
					enqueueMessage($params->{message});
				}
				my $response = HTTP::Response->new(200);
				my $header = HTTP::Headers->new;					

				$header->header('Content-Type' => 'text/html; charset=UTF-8'); 
				$c->send_basic_header();
				#$response->header($header);
				$response->content( getHttpResponse($params));
				#printServerInfo(Dumper($response));
				$c->send_response($response);
				#print Dumper($response);
			} else {
				$c->send_error(RC_FORBIDDEN)
			}
			$c->close;
			undef($c);
#		}
	}
	
}

sub parseUri{
	my $uri=shift;
	my $params={};
	my @params=split(/\?/,$uri);
	shift @params;
	$uri=join('?',@params);
	foreach my $pair (split(/\&/,$uri)){
		my ($key,$value)=split(/\=/,$pair);
		next unless defined $key;
		next unless defined $value;
		$value=uri_unescape($value);
		$value=~s/\+/ /g;
		$params->{$key}=$value;
	}
	return $params;
}

sub enqueueMessage{
	my $message=shift;
	open my $file, '>:utf8', $message_file;
	print $file $message."\n";
	close $file;
}

sub formatTime{
	my $time=shift;
	return strftime("%Y-%m-%d %H:%M:%S", localtime($time));
}

sub printInfo{
	my $level=0;
	$level=shift @_ if(@_==2);
	#." ".sprintf("%02f",time())
	print(formatTime(time())." -- INFO   -- ".$_[0]."\n") if($debug>=$level);
}

sub printServerInfo{
	my $level=0;
	$level=shift @_ if(@_==2);
	#." ".sprintf("%02f",time())
	print("\n".formatTime(time())." -- SERVER -- ".$_[0]."\n") if($debug>=$level);
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



sub getHttpResponse{
	my $params=shift;
	printServerInfo("logfile:$log_file");
	my $log=(`tail -n 200 $log_file`);
	$log=~s/\&</\&amp;/g;
	$log=~s/\</\&lt;/g;
	$log=~s/\>/\&gt;/g;
	my $message=$params->{message}||'';

	my $log_style='display:none';
	if((defined $params->{log})&&($params->{log}eq'show')){
		$log_style='display:block';
	}

	return qq{<!DOCTYPE html>
<html>
<head>

<style>
body{
	margin:0;
	padding:0;
	background:#ccc;
	font-family:Arial,Helvetica,Verdana,sans-serif
}

h1,h2,h3{
	text-align:center;
	font-weight:normal;
	font-size:3em;
	margin:0.5em;
}

#panel{
	width:90%;
	padding:1em;
	background:#fff;
}

input.field{
	width:600px;
	border:1px solid gray;
	border-radius:3px;
	padding:1em;
}

input.submit{
	padding:1em;
	width:10em;
}

#log{
	white-space: pre;
	font-family:monospace;
	background:gray;
	width:90%;
	height:400px;
	overflow:auto;
	text-align:left;
	padding:1em;
}

#button_bar{
	text-align:left;
	width:90%;
}
</style>

<script>
	function scrollLog(){
		var log=document.getElementById('log');
		log.scrollTop=10000;
		if(window.document.location.href.indexOf("?log=show") > -1){
			window.setTimeout("updateLog()", 10000);
		}
	}

	function showLog(){
		var log=document.getElementById('log');
		log.setAttribute('style', 'display:block');
		scrollLog();
	}

	function hideLog(){
		log.setAttribute('style', 'display:none');
		window.document.location='?';
	}

	function updateLog(){
		window.document.location='?log=show';
	}
</script>
</head>

<body onload="scrollLog()">
	<center>
		<div id="panel">
			<h3>send a message</h3>
			<form>
				<input class="field" name="message" value="$message"/>
				<input class="submit" type="submit" value="submit">
			</form>
			<br>It can take up to a minute until the message is shown...
		</div>
		<br>
		<div id="button_bar">
			Log: 
			<button onclick="showLog()">show</button>
			<button onclick="hideLog()">hide</button>
			<button onclick="updateLog()">update</button>
		</div>
		<div id="log" style="$log_style">$log</div>
	</center>
</body>
</html>
};
}

