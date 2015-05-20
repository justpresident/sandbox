#!/usr/bin/perl

use warnings;
use strict;

use File::Slurp; # libfile-slurp-perl
use Data::Dumper;

my $file_name = shift || glob($ENV{HOME}."/Dropbox/*pps/WorkFlowy/WorkFlowy*.txt");


my @lines = read_file($file_name);

my $tree = parse_workflowy(\@lines);
#print Dumper($tree);

#print Dumper($tree->{Work}->{TREE}->{AntiFraud}->{TREE}->{CRITICAL});

#print_tree($tree->{Work}->{TREE}->{AntiFraud}->{TREE}->{CRITICAL});
#print "-----\n";
#print dump_workflowy($tree->{Work}->{TREE}->{AntiFraud}->{TREE}->{CRITICAL});
print dump_workflowy($tree, 'html');
#print_tree($tree);


sub dump_workflowy {
	my $tree = shift;
	my $format = shift || 'plain';
	my $indent = shift || '';

	my $result = '';
	if($tree->{INFO}) {
		$tree->{INFO} =~ s/(\@\S+)/<span class=who>$1<\/span>/g if $format eq 'html';
		
		$result .= "<span class=note>" if $format eq 'html';
		$result .= "$indent  " . $tree->{INFO}."\n";
		$result .= "</span>" if $format eq 'html';
	};

	my $elem_prefix = "- ";
	my $elem_postfix = "";
	if ($format eq 'html') {
		$elem_prefix = "<li>";
		$elem_postfix = "</li>";
	}

	return $result unless $tree->{TREE};
	
	$result .= "$indent<ol>\n" if $format eq 'html';
	foreach my $key (keys %{$tree->{TREE}}) {
		my $print_key = $key;
		$print_key =~ s/(\@\S+)/<span class=who>$1<\/span>/g if $format eq 'html';
		$result .= $indent . "    $elem_prefix$print_key\n";
		$result .= "<br>" if $format eq 'html';
		$result .= dump_workflowy($tree->{TREE}->{$key}, $format, "$indent    ");
		$result .= "$indent    $elem_postfix\n";
	}
	$result .= "$indent</ol>\n" if $format eq 'html';


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

			$result->{TREE}->{$key} = undef;
			if ($i < scalar(@$lines) - 1) {
				if ($lines->[$i + 1] =~ /^$indent\s*"/) {
					# parse info
					$i++;
					while(1) {
						my $line = $lines->[$i];
						chomp $line;
						$line =~ s/^\s+//;
						$result->{TREE}->{$key}->{INFO} .= $line.' ';
						last if $line =~ /"$/;
						$i++;
					}
				}
			}
			if ($i < scalar(@$lines) - 1) {
				# parse recursive if next line has bigger indent
				my ($new_indent) = $lines->[$i+1] =~ /^(\s*)/;
				if (length($new_indent) > length($indent)) {
					$result->{TREE}->{$key}->{TREE} = parse_workflowy($lines, $i+1);
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

