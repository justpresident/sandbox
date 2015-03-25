#!/usr/bin/perl

use warnings;
use strict;

use File::Slurp; # libfile-slurp-perl
use Data::Dumper;

my $file_name = glob($ENV{HOME}."/Dropbox/*pps/WorkFlowy/WorkFlowy*.txt");


my @lines = read_file($file_name);

my $tree = parse_workflowy(\@lines);

print Dumper($tree->{Work}->{TREE}->{AntiFraud}->{TREE}->{CRITICAL});

print_tree($tree->{Work}->{TREE}->{AntiFraud}->{TREE}->{CRITICAL});
print "-----\n";
print dump_workflowy($tree->{Work}->{TREE}->{AntiFraud}->{TREE}->{CRITICAL});

sub dump_workflowy {
	my $tree = shift;
	my $indent = shift || '';

	my $result = '';
	if($tree->{INFO}) {
		$result .= "$indent  " . $tree->{INFO}."\n";
	};

	return $result unless $tree->{TREE};
	foreach my $key (keys %{$tree->{TREE}}) {
		$result .= $indent . "    - $key\n";
		$result .= dump_workflowy($tree->{TREE}->{$key}, "$indent    ");
	}

	return $result;
}

sub print_tree {
	my $tree = shift;
	my $indent = shift || "";

	return unless $tree;

	if($tree->{INFO}) {
		print "$indent  " . $tree->{INFO}."\n";
	};
	return unless $tree->{TREE};
	foreach my $key (keys %{$tree->{TREE}}) {
		print $indent . "    - $key\n";
		print_tree($tree->{TREE}->{$key}, "$indent    ");
	}
}

sub parse_workflowy {
	my $lines = shift;
	my $index = shift || 0;

	my ($indent) = $lines->[$index] =~ /^(\s*)/;
	my $result = {};
	for (my $i = $index; $i < scalar(@$lines); $i++) {
		my $line = $lines->[$i];
		chomp($line);
		if ($line =~ /^$indent-/) {
			my $key = $line;
			$key = trim($line);

			$result->{$key} = undef;
			if ($i < scalar(@$lines) - 1) {
				if ($lines->[$i + 1] =~ /^$indent\s*"/) {
					# parse info
					$i++;
					while(1) {
						my $line = $lines->[$i];
						chomp $line;
						$line =~ s/^\s+//;
						$result->{$key}->{INFO} .= $line;
						last if $line =~ /"$/;
						$i++;
					}
				}
			}
			if ($i < scalar(@$lines) - 1) {
				# parse recursive if next line has bigger indent
				my ($new_indent) = $lines->[$i+1] =~ /^(\s*)/;
				if (length($new_indent) > length($indent)) {
					$result->{$key}->{TREE} = parse_workflowy($lines, $i+1);
					$i++ while $i < scalar(@$lines) - 1 && $lines->[$i + 1] =~ /^$new_indent/;
				}
			}
		} else {
			last;
		}
	}

	return $result;
}

sub trim {
	my $str = shift;

	$str =~ s/^\s*-\s//;
	$str =~ s/\s+$//;
	return $str;
}

