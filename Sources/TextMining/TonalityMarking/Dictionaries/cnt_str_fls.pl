#! /usr/bin/perl
use strict;
use warnings;
sub usage {
  print "\nCount of string in files\n";
  print "\nUse: $0 maskfile\n";
  print "outfile cnt_str_fls\n"; exit 1; };
#########################################################
  usage if ( $#ARGV < 0);

  my $mask = $ARGV[0];
  opendir(DD, ".") || die;
  my @files = readdir DD;
  my @ch = grep/\S+\.$mask/, @files;
  my $siz=@ch;

###########################################################
  my $nmbs=0;
  my $out_file = "cnt_str_fls";
  open( F_OUT, "+>>$out_file" ) || die "Can't open file: $!";
  print F_OUT "name file\tnumber words\n";

  print "Let's start on...\n";
  for (my $t=0; $t<$siz; $t++)
  {
    open( F_IN,  $ch[$t] ) or die "Can't open file: $!";
    print $ch[$t],"\n";
    my $nmb=0;
    while (my $line = <F_IN> ) {
      $nmb++;
    }
    close( F_IN );
    $nmbs = $nmbs+$nmb;
    print F_OUT "$ch[$t]\t$nmb\n";
  }
  closedir(DD);
  print F_OUT "summa\t$nmbs";
  close( F_OUT );



