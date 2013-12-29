#!/usr/bin/perl

# avconv -i /media/daten/video_produktion/firecops/heroin-640x360x160x2000_17.avi -s 64x8 -vcodec v410 -an -y test.avi
# avconv -i oSCX78-8-q0.mp4 -s 64x8 -vcodec v410 -an -y test.avi
# avconv -s 100x50 -vcodec bmp -an -y test.avi -i test.mpg
use Encode;
use Data::Dumper;
use Time::HiRes qw(time);
use Getopt::Long;
use strict;
use warnings;

my $xmax = 64;
my $ymax = 8;
my $start= 0;
my $end  = 999999999;
my $filename='test.avi';
my $play=0;

my $out_file=undef;
my $data_content='';

$SIG{INT}=sub(){
	print $out_file $data_content;
	close $out_file;
	print "\e[23;0H";
	exit;
};

GetOptions (
	'input|i=s' => \$filename,
	'xmax|x=i'  => \$xmax,
	'ymax|y=i'  => \$ymax,
	'start|s=i' => \$start,
	'end|e=i'   => \$end,
	'play|p'    => \$play
);

#print Dumper $filename;
#exit;
open my $in_file, '<', $filename or die "Cannot open file '$filename' for reading";
binmode $in_file;

open $out_file, ">$filename"."clock"  or die "Cannot open file '$filename.clock' for writing";
binmode $out_file;

#$|=1;

my $buffer;

print "\033[2J"; 
clear_screen();

my $shift=[128,64,32,16,8,4,2,1];

my $broken_image='';
my $c=0;
my $time=time();
while ( my $got = read( $in_file, $buffer, 50000000) ) {
	my @images=split/00dc/,$buffer;
	#print @images."\n";
	if ($broken_image ne ''){
		$images[0]=$broken_image.$images[0];
		$broken_image='';
	}
	for my $i (0..@images-2){
		my $image=$images[$i];
		draw_image($image, $c);
		$c++;
	}
	my $image=$images[-1];
	my $image_size=draw_image($image, $c);
	if($image_size<0){
		$broken_image=$image;
		$c++;
	}else{
		$broken_image='';
	}
}

print $out_file $data_content;

#print "images:$c\n";

sub draw_image{
	my $image=$_[0];
	my $nr   =$_[1];
	return if($nr==0);
	return if ($nr<$start);
	if ($nr>$end){
		close $in_file;
		close $out_file;
		exit;
	}

	return -1 if (length($image)<50);
	my $start=4;
	#for my $i(10..28){print sprintf("%02i",$i)." ".ord(substr($image,$i ,1))."\n";}
	my $offset    = ord(substr($image,$start+10,1));
	my $width     = ord(substr($image,$start+18,1))+ord(substr($image,$start+19,1))*256;
	my $height    = ord(substr($image,$start+22,1))+ord(substr($image,$start+23,1))*256;
	my $colorByte = ord(substr($image,$start+28,1))/8;
	my $size      = $width*$height*$colorByte;
	
	return if ($width<1);
	return if ($height<1);
	return -1 if (length($image)<$size+$offset);

	my $max  = 256;
	my $low  = $max/3;
	my $high = $max*2/3;

	$low=64;
	#$high=96;

	my $pos=0;
	my $y=0;
	my $x=0;

	#init resample buffer
	my $rlines=[];
	my $glines=[];
	my $blines=[];
	for (my $y=0;$y<$ymax;$y++){
		for (my $x=0;$x<$xmax;$x++){
			$rlines->[$y]->[$x]=0;
			$glines->[$y]->[$x]=0;
			$blines->[$y]->[$x]=0;
		}
	}

	#scale
	for (my $y=0;$y<$height;$y++){
		my $ypos=int($y*$ymax/$height);
		my $rline=$rlines->[$ypos];
		my $gline=$glines->[$ypos];
		my $bline=$blines->[$ypos];
		for (my $x=0;$x<$width;$x++){
			#$pos=$colorByte*(int($x)+int($y*$width));
			$pos=$colorByte*(int(($x+$width)/2)+int($y*$width));
			my $or=(ord(substr($image,$offset+$pos  ,1)));
			my $og=(ord(substr($image,$offset+$pos+1,1)));
			my $ob=(ord(substr($image,$offset+$pos+2,1)));

			my $r=($or+$ob)/2;
			my $g=($og+$ob)/2;
			#my $b=($or+$og+$ob)/3;

			my $xpos=int($x*$xmax/$width);
			$rline->[$xpos]=1 if($r>$low);
			$gline->[$xpos]=1 if($g>$low);

			#$rline->[$xpos]=2 if($r>$high);
			#$gline->[$xpos]=2 if($g>$high);
		}
	}

	#encode
	my @drawbuffer1=();
	my @drawbuffer2=();
	for my $y(0..7){
		for my $x(0..7){
			my $addr=($y *16) + $x;
			$drawbuffer1[$addr  ] =0;
			$drawbuffer2[$addr  ] =0;
			$drawbuffer1[$addr+8] =0;
			$drawbuffer2[$addr+8] =0;
		}
	}

	my $out='';
	my $print_stat=($nr%25==0);

	for my $y(0..7){
		my $rline=$rlines->[7-$y];
		my $gline=$glines->[7-$y];
		my $bline=$blines->[7-$y];
		for my $x(0..7){
			my $rbyte1=0;
			my $rbyte2=0;
			my $gbyte1=0;
			my $gbyte2=0;
			my $addr=$x*8;
			for my $bit(0..7){
				$rbyte1+=$shift->[$bit] if ($rline->[$addr+$bit]==1);
				#$rbyte2+=$shift->[$bit] if ($rline->[$addr+$bit]==2);
				$gbyte1+=$shift->[$bit] if ($gline->[$addr+$bit]==1);
				#$gbyte2+=$shift->[$bit] if ($gline->[$addr+$bit]==2);
			}
			my $addr=$y*16+$x;
			$drawbuffer1[$addr  ]=$rbyte1;
			$drawbuffer1[$addr+8]=$gbyte1;
			$drawbuffer2[$addr  ]=$rbyte2;
			$drawbuffer2[$addr+8]=$gbyte2;
		}
		$out.=	 join("\n",join('',@$rline))."\t"
			.join("\n",join('',@$gline))."\t"
			.join("\n",join('',@$bline))."\n"
		if($print_stat);
	}

	if($print_stat){
		clear_screen();

		$out.= "size1: ".@drawbuffer1."\n";
		$out.= "size2: ".@drawbuffer2."\n";
		for my $i (0..@drawbuffer1-1){
			$out.=sprintf("%03d:%03d ",$i, $drawbuffer1[$i])
		}
		$out.="\n";
	
		for my $i (0..@drawbuffer2-1){
			$out.=sprintf("%03d:%03d ",$i, $drawbuffer2[$i])
		}
		$out.="\n";
		$out.="file:$filename image:>$nr< offset:$offset width:$width height:$height colorByte:$colorByte size:$size time:".(time()-$time)."\n";
		$time=time();
		print $out;
	}

	my $data1=pack('C*',@drawbuffer1);
	my $data2=pack('C*',@drawbuffer2);
	#$data1=~s/\x0a/\x08/g;
	#$data2=~s/\x0a/\x08/g;

	$data_content.=	 "a".$data1."\n"
#	."b".$data2."\n"
	;

	#print Dumper(\@drawbuffer1);
	select(undef, undef, undef, 1.0/30.0) if ($play);
}

sub clear_screen{
	#print "\033[2J"; 
	print "\e[0;0H";
}

