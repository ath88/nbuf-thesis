#!/usr/bin/env perl

use Modern::Perl;
use File::Slurp;


{
    next;
    my $filename = 'data/50G.dat';
    unlink $filename;
    say 'Writing 50G of data to "50G.dat"';
    for (1..50) {
        write_file('data/50G.dat', {binmode => ':raw', append => 1}, '1' x (1024 * 1024 * 1024));
    }
}

{
    next;
    my $filename = 'data/50G.dat';
    unlink $filename;
    say 'Writing 50G of data to "50G.dat"';
    for (1..50) {
        write_file('data/50G.dat', {binmode => ':raw', append => 1}, '1' x (1024 * 1024 * 1024));
    }
}

say 'Writing 1G of data to "1G.dat"';
write_file('data/1G.dat', {binmode => ':raw'}, '1' x (1024 * 1024 * 1024));

say 'Writing 100M of data to "10m.dat"';
write_file('data/100M.dat', {binmode => ':raw'}, '1' x (1024 * 1024 * 100));

say 'Writing 10M of data to "10m.dat"';
write_file('data/10M.dat', {binmode => ':raw'}, '1' x (1024 * 1024 * 10));

say 'Writing 10kb of data to "10kb.dat"';
write_file('data/10kb.dat', {binmode => ':raw'}, '1234' x (1024 * 10 / 4));

say 'Writing 1b of data to "1b.dat"';
write_file('data/1b.dat', {binmode => ':raw'}, '1' x (1));