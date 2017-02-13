#! perl -w
##! /usr/bin/perl
use strict;
use warnings;
use utf8;
use Fcntl ':seek';
use Benchmark;
my $t0 = new Benchmark;

sub usage {
	print "\nShow numbers of string and file's names of double words in files\n";
	print "\nUse: $0 maskfile\n";
	print "log_file dbl2wrd.log\n"; exit 1; };
#########################################################
usage if ( $#ARGV < 0);
###########################################################
my  $mask = $ARGV[0];
opendir(DD, ".") || die;
my  @files = readdir DD;
my  @ch = grep/\S+\.$mask/, @files;
my  $siz=@ch;
##########################################################

my $out_file="dbl2wrd.log";
open F_OUT, '>', $out_file;
binmode F_OUT, ':utf8';

for (my $t=0; $t<$siz-1; $t++){
	open( FILE,  $ch[$t] ) or die "Can't open file: $!";
	binmode FILE, ':utf8';
	#sysread F_IN, $s, 1;
        seek FILE, 3, 0; #пропускаем первые три байта
        print "$ch[$t]\n";
	my @arr = ();
  	while ( my $txt = <FILE> ){
		if ($txt =~ m /(\S+)/){
			push @arr,$txt;
		}
      	}
      	close FILE;

      	for (my $f = $t+1; $f<$siz; $f++) {
	      	open( F_IN,  $ch[$f] ) or die "Can't open file: $!";
			binmode F_IN, ':utf8';
			#sysread F_IN, $s, 1;
			print "\t$ch[$f]\n";
        	seek F_IN, 3, 0; #пропускаем первые три байта
        	my @wrd;
        	while ( my $str = <F_IN> ){
				if ($str =~ m /(\S+)/){
					push @wrd,$str;
				}
			}
			close F_IN;
			foreach my $arr(@arr){
				if ($arr =~ /\t/){
					$arr=~ m/([-*\#\w ]+)\t(\d)/g;
					$arr = $1;
				}
				else {
					$arr =~ s/\n//;
				}
				foreach my $wrd(@wrd){
				my $str;
					if ($wrd =~ m /(\S+)/){
						if ($wrd =~ /\t/){
							$wrd =~ m/([-*\#\w ]+)\t(\d)/g;
							$wrd = $1;
						}
						else {
							$wrd =~ s/\n//;
						}
						if ($wrd eq $arr){
							print F_OUT "$ch[$t]\t$ch[$f]\n";
							print F_OUT "$arr\n";
							print "$ch[$t]\t$ch[$f]\n";
							print "$arr\n";
						}
					}
				}
			}

      	}
}
close F_OUT;
my $t1 = new Benchmark;
print timestr (timediff ($t1, $t0)), "\n";
