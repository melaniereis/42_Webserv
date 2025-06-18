#!/usr/bin/perl
use strict;
use warnings;

# Function to decode URL-encoded strings
sub decode_url {
    my $str = shift;
    $str =~ s/\+/ /g;
    $str =~ s/%([0-9A-Fa-f]{2})/chr(hex($1))/eg;
    return $str;
}

# Function to escape HTML
sub escape_html {
    my $str = shift;
    $str =~ s/&/&amp;/g;
    $str =~ s/</&lt;/g;
    $str =~ s/>/&gt;/g;
    $str =~ s/"/&quot;/g;
    return $str;
}

# Parse query parameters
my $query = $ENV{'QUERY_STRING'} || '';
my %params;
foreach my $pair (split /&/, $query) {
    my ($key, $value) = split /=/, $pair, 2;
    $params{$key} = decode_url($value) if defined $key && defined $value;
}

my $name = $params{'name'} || '';
my $escaped_name = escape_html($name);

# Print HTTP header
print "Content-Type: text/html\r\n\r\n";

# Begin HTML
print <<"HTML";
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>CGI Greeting</title>
    <style>
        :root {
            --primary: #6C63FF;
            --accent: #00C9A7;
            --dark: #121826;
            --light: #F0F5FF;
            --shadow: 0 10px 30px rgba(0, 0, 0, 0.3);
            --transition: all 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275);
        }

        body {
            background: linear-gradient(to bottom, #0a0e17, var(--dark));
            color: var(--light);
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            margin: 0;
        }

        .card {
            background: rgba(255, 255, 255, 0.05);
            border-radius: 16px;
            padding: 2rem 3rem;
            box-shadow: var(--shadow);
            text-align: center;
            backdrop-filter: blur(15px);
            max-width: 500px;
            width: 100%;
            opacity: 0;
            transform: translateY(30px);
            transition: var(--transition);
        }

        .card.show {
            opacity: 1;
            transform: translateY(0);
        }

        h1 {
            font-size: 2.5rem;
            background: linear-gradient(to right, var(--primary), var(--accent));
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            margin-bottom: 1rem;
        }

        .subtitle {
            color: #ccc;
            margin-bottom: 2rem;
        }

        .form input[type="text"] {
            padding: 0.6rem 1rem;
            font-size: 1rem;
            border-radius: 8px;
            border: none;
            margin-right: 0.5rem;
            outline: none;
            background: #1e263b;
            color: white;
        }

        .form input[type="submit"] {
            padding: 0.6rem 1.2rem;
            background: var(--primary);
            border: none;
            color: white;
            border-radius: 8px;
            cursor: pointer;
            transition: var(--transition);
        }

        .form input[type="submit"]:hover {
            background: var(--accent);
            transform: translateY(-3px);
        }

        .greeting {
            font-size: 1.4rem;
            margin-bottom: 2rem;
        }
    </style>
</head>
<body>
    <div class="card" id="greet-card">
        <h1>👋 Welcome</h1>
        <div class="subtitle">A modern CGI greeting with Perl</div>
HTML

# Conditional greeting or form prompt
if ($name ne '') {
    print qq{<div class="greeting">Hello, <strong>$escaped_name</strong>! 🎉</div>\n};
} else {
    print qq{<div class="greeting">Enter your name below to get greeted:</div>\n};
}

# Form HTML
print <<'FORM';
        <form class="form" method="get">
            <input type="text" name="name" placeholder="Enter your name" required />
            <input type="submit" value="Greet Me!" />
        </form>
    </div>
    <script>
        document.addEventListener('DOMContentLoaded', () => {
            const card = document.getElementById('greet-card');
            setTimeout(() => {
                card.classList.add('show');
            }, 100);
        });
    </script>
</body>
</html>
FORM
