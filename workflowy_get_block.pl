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
print dump_workflowy($tree, 'wiki');
#print_tree($tree);


sub dump_workflowy {
	my $tree = shift;
	my $format = shift || 'plain';
	my $indent = shift || '';

	my $result = '';
	if($tree->{INFO}) {
		$tree->{INFO} = format_str($tree->{INFO}, $format);
		$result .= mk_info($tree->{INFO}, $indent, $format);
	};

	my ($elem_prefix, $elem_postfix) = mk_prefixes($format, $indent);

	return $result unless $tree->{TREE};
	
	$result .= "$indent<ol>\n" if $format eq 'html';
	foreach my $key (keys %{$tree->{TREE}}) {
		my $print_key = $key;
		$print_key = format_str($print_key, $format, "item");
		$result .= "$elem_prefix$print_key\n";
		$result .= "<br>" if $format eq 'html';
		$result .= dump_workflowy($tree->{TREE}->{$key}, $format, "$indent    ");
		$result .= "$elem_postfix\n";
	}
	$result .= "$indent</ol>\n" if $format eq 'html';


	return $result;
}

sub mk_prefixes {
	my $format = shift;
	my $indent = shift;
	
	my $elem_prefix  = "$indent    - ";
	my $elem_postfix = "$indent    ";
	if ($format eq 'html') {
		$elem_prefix = "<li>";
		$elem_postfix = "</li>";
	} elsif ($format eq 'wiki') {
		$elem_prefix = "$indent";
		$elem_postfix = "";
		
	}

	return ($elem_prefix, $elem_postfix);
}

sub mk_info {
	my $str = shift;
	my $indent = shift;
	my $format = shift;

	my $result = "$indent  " . $str . "\n";
	if ($format eq 'html') {
		$result = "<span class=note>$indent $result\n</span>";
	} elsif ($format eq 'wiki') {
		$result = "<[$str\n]>";
	}
	return $result;
}

sub format_str {
	my $str = shift;
	my $format = shift;
	my $type = shift || "note";

	if ($format eq 'html') {
		$str =~ s/(\@\S+)/<span class=who>$1<\/span>/g;
		$str =~ s/\[COMPLETE\]\s*(.+)/<span class=complete>$1<\/span>/;
	} elsif ($format eq 'wiki') {
		$str =~ s/(\@\S+)/!!(blue)$1!!/g;
		$str =~ s/\[COMPLETE\]\s*(.+)/!!(green)**\\\/**!! $1/;
		if ($type eq "item") {
			$str = "====$str====";
		}
	}

	return $str;
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

