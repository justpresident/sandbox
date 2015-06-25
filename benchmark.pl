#!/usr/bin/perl

use warnings;
use strict;
use autodie;

use Data::Dumper;
use Benchmark qw(cmpthese timethese);

#my $MAX = 4294967295;
my $MAX = 100000;
my @DICT = map{naive($_)}(0..255);

my $r = timethese(5, {
		naive => sub{ test(\&naive) },
		with_dict => sub{ test(\&with_dict) },
		find1 => sub{ test(\&find1) },
});

cmpthese($r);

sub test {
	my $func = shift;
	for(my $i = 0; $i < $MAX; $i++) {
		$func->($i);
	}
}

sub naive {
	my $number = shift;

	my $bit_count = 0;
	while($number > 0) {
			$bit_count += ($number % 2);
			$number = $number >> 1; 
	}
	return $bit_count;
}

sub find1 {
	my $number = shift;

	my $bit_count = 0;
	while($number > 0) {
		$number &= ($number-1);
		$bit_count++;
	}
	return $bit_count;
}

sub with_dict {
	my $number = shift;

	my $bit_count = 0;
	while($number > 0) {
		$bit_count += $DICT[$number & 0xff];
		$number >>= 8;
	}
	return $bit_count;
}

