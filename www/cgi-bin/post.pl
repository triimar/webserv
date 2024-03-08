#!/usr/bin/perl
use strict;
use warnings;

# Print the HTTP header
print "Content-type: text/plain\n\n";

# Check the REQUEST_METHOD
my $request_method = $ENV{'REQUEST_METHOD'} || '';
if ($request_method ne 'POST') {
    print "Error: This script only accepts POST requests.\n";
    exit;
}

# Read from STDIN and write to STDOUT
while (<STDIN>) {
    print $_;
}
