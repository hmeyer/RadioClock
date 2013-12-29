#!/usr/bin/perl

my $filename=$ARGV[0];
if ($filename eq''){
     print "USAGE: $0 character.set";
}
unless (-r $filename){
     print "ERROR: cannot read '$filename'\n";
}
open (my $file, '<',$filename);
my $content=join("",(<$file>));
close $file;

print "#include <avr/io.h>\n";
print "#include <avr/pgmspace.h>\n";
print "extern const unsigned char characters[] PROGMEM;\n";
print "const unsigned char characters[] = {\n";

my @names=();
my $char_count=32;
while($content=~m/(^|\n)Character\(([^\)]+)/g){
     print (($char_count==32)?' ':',');
     my $char=$2;
     my @lines=split(/\n/,$char);

     my $name="c_".sprintf('%03d',$char_count);
     push @names,$name;

     my @vals=();
     my $size=0;
     for my $line (@lines){
         if ($line=~/\"([^\"]+)/){
             my $val=$1;
		if(length($line)!=7){
			print STDERR "ALERT: $char_count ".length($line)." ".$line."\n";
		}

	     $size=5 if(getbit($val,4) && ($size<5));
	     $size=4 if(getbit($val,3) && ($size<4));
             $size=3 if(getbit($val,2) && ($size<3));
             $size=2 if(getbit($val,1) && ($size<2));
             $size=1 if(getbit($val,0) && ($size<1));
         }
     }
     $size=1 if($size==0);
     $size++;
     push @vals2, '0x'.sprintf('%02x',$size);

     for my $line (@lines){
         if ($line=~/\"([^\"]+)/){
             my $val=$1;
             $val=
               ( getbit($val,0) <<7 )
             | ( getbit($val,1) <<6 )
             | ( getbit($val,2) <<5 )
             | ( getbit($val,3) <<4 )
             | ( getbit($val,4) <<3 )
             ;
             push @vals, '0x'.sprintf('%02x',$val);
         }
     }
     print join(', ',@vals).$lines[0]."\n";
     $char_count++;
}
print ",".join(', ',@vals2).$lines[0]."\n";


print "};\n";

sub getbit{
     my $s=shift;
     my $pos=shift;
     my $char=substr($s,$pos,1);
     return 1 if ($char ne ' ');
     return 0;
}

