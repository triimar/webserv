#!/usr/bin/perl
use strict;
use warnings;
use URI::Escape;
use HTML::Entities;

# Read the request method and content length from the environment
my $request_method = $ENV{'REQUEST_METHOD'} || '';
my $content_length = $ENV{'CONTENT_LENGTH'} || 0;
my $content_type   = $ENV{'CONTENT_TYPE'} || '';

# Process the input data
print "Content-type: text/html; charset=utf-8\n\n";
print <<HTML;
<html>
<head>
    <title>CGI Script Result</title>
    <style>
        body {
            font-family: 'Arial', sans-serif;
            background-color: #f5f5f5;
            color: #333;
            margin: 20px;
        }
        h1 {
            color: #007bff;
        }
        ul {
            list-style-type: none;
            padding: 0;
        }
        li {
            background-color: #fff;
            padding: 10px;
            margin-bottom: 10px;
            border-radius: 5px;
            box-shadow: 0 0 5px rgba(0, 0, 0, 0.1);
        }
        strong {
            color: #007bff;
        }
    </style>
</head>
<body>
HTML

if ( $request_method ne 'POST' ) {
    print "<p>Expects POST request</p>";
}
elsif ( $content_length == 0 ) {
    print "<p>Expects content</p>";
}
elsif ( $content_type ne 'application/x-www-form-urlencoded' ) {
    print "<p>Invalid content type, expected: application/x-www-form-urlencoded</p>";
}
else {
    # Read the input data from stdin based on content length
    my $input_data;
    read( STDIN, $input_data, $content_length );

    # Parse form data
    my %params;
    my @pairs = split( /&/, $input_data );
    foreach my $pair (@pairs) {
        my ( $key, $value ) = split( /=/, $pair );
        $value = uri_unescape($value);  # Decode URL encoding
        $value = encode_entities($value);  # Encode special characters
        $params{$key} = $value;
    }

    # Display form parameters in pretty HTML
    print "<h1>Form Submission Results</h1>";
    print "<ul>";

    my @display_params = ( 'name', 'email', 'age', 'color', 'fruit', 'hobbies', 'subscribe' );
    foreach my $param (@display_params) {
        my $value = $params{$param} || '';
        print "<li><strong>$param:</strong> $value</li>";
    }

    print "</ul>";
}

print <<HTML;
</body>
</html>
HTML
