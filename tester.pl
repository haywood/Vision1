#!/usr/bin/perl

system('make', 'all');
$threshold=111;

system("./p1 two_objects.pgm $threshold test_binary.pgm");
system("./p2 test_binary.pgm test_labeled.pgm");
system("./p3 test_labeled.pgm test_db test_lines.pgm");
system("./p4 test_labeled.pgm test_db test_recog1.pgm");
