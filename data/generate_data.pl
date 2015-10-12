#!/usr/bin/env perl

use Modern::Perl;
use File::Slurp;


my %sizes = (
    b  => 1,
    KB => 1024,
    MB => 1024 * 1024,
    GB => 1024 * 1024 * 1024,
);

my $dir = 'data';
my $ext = 'dat';

if (defined $ARGV[0]) {
    my $input = $ARGV[0];

    $input =~ m/(\d+)(\w+)/;

    unless (defined $1 && defined $2) {
        say 'Wrong input. Expecting regex: (\d+)(\w+)';
        exit(0);
    }

    if ($1 < 1 || $1 > 1024) {
        say 'Specify number strictly between 0 and 1024';
        exit(0);
    }

    unless (defined $sizes{$2}) {
        say 'Specify unit as [b|KB|MB|GB]';
        exit(0);
    }

    say "Writing $input of data to $dir/$input.$ext";

    my $filename = "$dir/$input.$ext";
    unlink $filename;

    for (1..$1) {
        write_file($filename, {binmode => ':raw', append => 1}, '1' x $sizes{$2});
    }
}

exit();