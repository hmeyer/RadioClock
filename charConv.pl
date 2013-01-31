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

print "#ifndef CHARSET_H\n";
print "#define CHARSET_H\n";
print "#include <avr/io.h>\n";
print "#include <avr/pgmspace.h>\n";
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


print "};\n";
print "#endif\n";

sub getbit{
     my $s=shift;
     my $pos=shift;
     my $char=substr($s,$pos,1);
     return 1 if ($char ne ' ');
     return 0;
}

