#!/usr/bin/perl

# logrotator.pl
#
# (C) 2006 Jonathan Disher <funjon@undernet.org>
#
# Threaded application to read chanfix log files from FIFO's, rotate them daily,
# and email them to the appropriate destinations.
#
# Requires:  perl 5.8.0 or higher with ithreads, Net::SMTP

require 5.8.0;
use Net::SMTP;
use strict;

##########################
##### FILE LOCATIONS #####
##########################

# Chanfix admin log
our $adminlog = "/home/chanfix/gnuworld/bin/chanfix-admin.log";

###########################
##### EMAIL ADDRESSES #####
###########################

# admin log destination
our $adminemail = 'abuse@mynet.org';

# Mail exchanger

our $MX = "mx.mynet.org";

#################################
##### OTHER IMPORTANT STUFF #####
#################################

our $pid = $$;
our $logdir = "/home/chanfix/gnuworld/bin";

system("mv $adminlog $adminlog.roll");
system("kill -HUP `pidof gnuworld`");

$adminlog .= ".roll";

&maillog($adminlog,$adminemail);

sub maillog($$)
{
  my ($logfile,$email) = @_;
  my ($line,$log);
  
  open(IN, "<$logfile") || die "Cannot open $logfile: $!\n";
  while ($line = <IN>) { $log .= $line; }
  close IN;

  my $smtp = new Net::SMTP($MX);
  $smtp->mail('chanfix@mynet.org');
  $smtp->to($email);
  $smtp->data($log);
  $smtp->quit;
}
