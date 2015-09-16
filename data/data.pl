#!/usr/bin/env perl

use Modern::Perl;
use File::Slurp;

say 'Writing 1G of data to "1G.dat"';
write_file('1G.dat', {binmode => ':raw'}, '1' x (1024 * 1024 * 1024));

say 'Writing 100M of data to "10m.dat"';
write_file('100M.dat', {binmode => ':raw'}, '1' x (1024 * 1024 * 100));

say 'Writing 10M of data to "10m.dat"';
write_file('10M.dat', {binmode => ':raw'}, '1' x (1024 * 1024 * 10));

say 'Writing 1b of data to "1b.dat"';
write_file('1b.dat', {binmode => ':raw'}, '1' x (1));