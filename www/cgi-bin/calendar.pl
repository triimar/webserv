#!/usr/bin/perl

# cal.pl - Display the calendar of a given month.
# Fedon Kadifeli, 1998 - April 2003.
# Improved by Trizen - February 2012

my (%months) = (
    '1'  => {LENGTH => 31, NAME => 'January'},
    '2'  => {LENGTH => 28, NAME => 'February'},
    '3'  => {LENGTH => 31, NAME => 'March'},
    '4'  => {LENGTH => 30, NAME => 'April'},
    '5'  => {LENGTH => 31, NAME => 'May'},
    '6'  => {LENGTH => 30, NAME => 'June'},
    '7'  => {LENGTH => 31, NAME => 'July'},
    '8'  => {LENGTH => 31, NAME => 'August'},
    '9'  => {LENGTH => 30, NAME => 'September'},
    '10' => {LENGTH => 31, NAME => 'October'},
    '11' => {LENGTH => 30, NAME => 'November'},
    '12' => {LENGTH => 31, NAME => 'December'},
);

my ($day, $real_month, $real_year) = (localtime time)[3 .. 5];
my ($month, $year) = ($real_month += 1, $real_year += 1900);

if (@ARGV and $ARGV[0] =~ /^(?:\d\d?|\w{3,})$/) {
    $month = shift @ARGV;
    if ($month =~ /^ *\d\d? *$/) {
        unless ($month >= 1 and $month <= 12) {
            die "Month must be between 1 and 12!\n";
        }
        $month = int $month;
    }
    else {
        while (my ($k, $v) = each %months) {
            if ($$v{'NAME'} =~ /^\Q$month\E/io) {
                $month = $k;
                last;
            }
        }
        $month = $real_month unless $month =~ /^\d\d?$/;
    }
}

if (@ARGV and $ARGV[0] =~ /^\d\d\d\d$/) {
    $year = int shift @ARGV;
}

print "Content-Type: text/html\n\n";  # Set the Content-Type header for HTML

print <<"HTML";
<!DOCTYPE html>
<html>
<head>
    <title>Calendar</title>
    <style>
        .highlight { background-color: yellow; }
    </style>
</head>
<body>
    <h1>$months{$month}{'NAME'} $year</h1>
    <table border="1">
        <tr>
            <th>Su</th>
            <th>Mo</th>
            <th>Tu</th>
            <th>We</th>
            <th>Th</th>
            <th>Fr</th>
            <th>Sa</th>
        </tr>
HTML

if ($year % 400 == 0 or $year % 4 == 0 and $year % 100 != 0) {
    $months{'2'}{'LENGTH'} = 29;
}
--$year;

my $st = 1 + $year * 365 + int($year / 4) - int($year / 100) + int($year / 400);

print "<tr><td colspan=\"", ($st % 7), "\"></td>";

foreach my $i (1 .. $months{$month}{'LENGTH'}) {
    if ($i == $day and $year == $real_year and $month == $real_month) {
        print '<td class="highlight">', $i, '</td>';
    }
    else {
        print '<td>', $i, '</td>';
    }

    if (($st + $i) % 7 == 0 and $i != $months{$month}{'LENGTH'}) {
        print "</tr><tr>";
    }
}

print "</tr></table></body></html>\n";
