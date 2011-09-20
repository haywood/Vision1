#!/usr/bin/perl

open MAKEFILE, '<', 'Makefile';
@lines=<MAKEFILE>;
close MAKEFILE;

for (0..255) {
	&loop($_, \@lines);
	system(make);
}

loop(111, \@lines);

sub loop {
   $n=shift;
	$lines=shift;
	open MAKEFILE, '>', 'Makefile';
	for (@$lines) {
		print MAKEFILE $_;
   	s/^THRESHOLD = \d+/"THRESHOLD = ".sprintf("%d", $n)/e;
	}
	close MAKEFILE;
}
