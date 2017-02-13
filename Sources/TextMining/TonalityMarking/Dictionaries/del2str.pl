#! /usr/bin/perl
use strict;
use warnings;
use utf8;
use Fcntl ':seek';
use Benchmark;
use Text::Iconv;

##########################################################
sub usage {
  print "Delete double string in files\n";
  print "Use: $0 mask\n";
  exit 1;
};
usage if ( $#ARGV != 0);

my  $mask = $ARGV[0];
opendir(DD, ".") || die;
my  @files = readdir DD;
my  @ch = grep/\S+\.$mask/, @files;
my $siz=@ch;
##########################################################
my $t0 = new Benchmark;
for (my $t=0; $t<$siz; $t++)
{
	my $cnt=0;
	my $s;
	open( F_IN,  $ch[$t] ) or die "Can't open file: $!";
	binmode F_IN, ':utf8';
	sysread F_IN, $s, 1;
        seek F_IN, 3, 0; #пропускаем первые три байта

	print $ch[$t],"\n";
	my %h = ();
	while (my $str = <F_IN> ){
		if ($str =~ /\t/){
			$str =~ m/([-*\#\w ]+)\t(\d)/g;
			my $wrd = $1;
            my $chf = $2;
			if(!exists( $h{$wrd})){
				%h -> {$wrd} = $chf;
			}
			else{
				$cnt++ ;
				my $converter = Text::Iconv->new("utf8", "cp1251");
				my $converted = $converter->convert($wrd);
				print "$converted\n";
			}
		}
		else{
            if(!exists( $h{$str})){
				%h -> {$str} = -1;
			}
			else{
				$cnt++ ;
			}
		}
	}
	close F_IN;
	print "$cnt\n";
	if ( keys(%h)>0 ){
       	open F_OUT, '>', $ch[$t];
       	binmode F_OUT, ':utf8';
       	syswrite F_OUT, $s, 1;
    	seek F_OUT, 3, 0;
		foreach my $k(sort keys %h){
			my $frq= $h{$k};
			if ($frq !=-1){
				print F_OUT "$k\t$frq\n";
			}
			else {
				print F_OUT "$k";
			}
		}
		close F_OUT;
	}
}
my $t1 = new Benchmark;
#my $td = timediff($t1, $t0);
#print "timestr($td),\n";
print timestr (timediff ($t1, $t0)), "\n";