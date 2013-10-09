#!/usr/bin/perl

my $filename='test.aviclock';
open my $out_file, ">$filename" or die "Cannot open file '$filename.clock' for writing";
binmode $out_file;

my $drawbuffer1=[];
my $drawbuffer2=[];

for my $i(0..255){
	for my $y(0..7){
		for my $x(0..63){
			my $addr=($y << 4) + ($x>>3);
			$drawbuffer1->[$addr  ] =0;
			$drawbuffer2->[$addr  ] =0;
			$drawbuffer1->[$addr+8] =0;
			$drawbuffer2->[$addr+8] =0;
		}
	}
	for my $y(0..7){
		for my $x(0..63){
			my $addr=($y << 4) + ($x>>3);
			$drawbuffer1->[$addr  ] =$i;
			$drawbuffer2->[$addr  ] =$i;
			$drawbuffer1->[$addr+8] =$i;
			$drawbuffer2->[$addr+8] =$i;
		}
	}

	my $data1=pack('C*',@$drawbuffer1);
	my $data2=pack('C*',@$drawbuffer2);
#	$data1=~s/\x0a/\x08/g;
#	$data2=~s/\x0a/\x08/g;
	print $out_file 
	 "a".$data1."\n"
	#."b".$data2."\n"
	;
}


