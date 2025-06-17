#!/usr/bin/perl
use warnings;

# Function to decode URL-encoded strings
sub decode_url {
    my $str = shift;
    $str =~ s/\+/ /g;                     # Replace '+' with space
    $str =~ s/%([0-9A-Fa-f]{2})/chr(hex($1))/eg;  # Decode %XX hex codes
    return $str;
}

# Function to escape HTML special characters
sub escape_html {
    my $str = shift;
    $str =~ s/&/&amp;/g;    # Escape &
    $str =~ s/</&lt;/g;     # Escape <
    $str =~ s/>/&gt;/g;     # Escape >
    $str =~ s/"/&quot;/g;   # Escape "
    return $str;
}

# Get the query string from the environment
my $query = $ENV{'QUERY_STRING'} || '';

# Parse the query string into a hash of parameters
my %params;
foreach my $pair (split /&/, $query) {
    my ($key, $value) = split /=/, $pair, 2;
    if (defined $key && defined $value) {
        $value = decode_url($value);
        $params{$key} = $value;
    }
}

# Extract the 'name' parameter, default to empty string if not provided
my $name = $params{'name'} || '';

# Output the HTTP header
print "Content-Type: text/html\r\n\r\n";

# Output the HTML content
print <<"HTML";
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>CGI Example</title>
    <style>
        body {
            background: #f7f9fa;
            font-family: 'Segoe UI', Arial, sans-serif;
            color: #222;
            margin: 0;
            padding: 0;
        }
        .container {
            max-width: 420px;
            margin: 60px auto;
            background: #fff;
            border-radius: 12px;
            box-shadow: 0 2px 16px rgba(0,0,0,0.07);
            padding: 2em 2.5em;
            text-align: center;
        }
        h1 {
            color: #3178c6;
            margin-bottom: 0.4em;
            font-size: 2.2em;
        }
        .subtitle {
            color: #666;
            margin-bottom: 1.5em;
        }
        .input-form {
            margin-top: 2em;
        }
        .input-form input[type="text"] {
            padding: 0.5em;
            font-size: 1em;
            border-radius: 6px;
            border: 1px solid #ccc;
            margin-right: 0.5em;
        }
        .input-form input[type="submit"] {
            background: #3178c6;
            color: #fff;
            border: none;
            border-radius: 6px;
            padding: 0.5em 1.2em;
            font-size: 1em;
            cursor: pointer;
        }
        .input-form input[type="submit"]:hover {
            background: #255a93;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>CGI Example</h1>
        <div class="subtitle">A simple Perl CGI greeting demo</div>
HTML

# Display greeting if name is provided, otherwise show the form prompt
if ($name ne '') {
    my $escaped_name = escape_html($name);
    print '<p style="font-size:1.3em;">Hello, <b>' . $escaped_name . '</b>!</p>';
} else {
    print '<p style="font-size:1.1em;">No name provided. Try entering your name below:</p>';
}

# Output the HTML form
print <<'FORM';
        <form class="input-form" method="get">
            <input type="text" name="name" placeholder="Enter your name" />
            <input type="submit" value="Greet Me!" />
        </form>
    </div>
</body>
</html>
FORM
