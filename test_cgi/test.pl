#!/usr/bin/perl
print "Content-Type: text/plain\r\n\r\n";
print "Hello from CGI! Query: $ENV{'QUERY_STRING'}\n";
print "Method: $ENV{'REQUEST_METHOD'}\n";
