#!/usr/bin/perl

use strict;
use warnings;
use URI::Escape;

# Get the QUERY_STRING from the environment variables
my $query_string = uri_unescape($ENV{'QUERY_STRING'} || '');

# Print the content type header for CGI
print "Content-type: text/html\n\n";

# Generate the HTML page with colorful styling
print <<HTML;
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            background-color: #f4f4f4;
        }

        main {
            padding: 20px;
        }

        .query-string {
            background-color: #3498db;
            color: #fff;
            padding: 10px;
            margin: 10px 0;
            font-size: 1.2em;
            text-align: center;
        }
    </style>
</head>

<body>
    <main>
        <div class="query-string">
            <strong>$query_string</strong>
        </div>
    </main>

</body>

</html>
HTML
