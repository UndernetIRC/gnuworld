#!/usr/bin/env python
#
# Script to check GNUWorld's mod.cservice translations for mismatched
# variable arguments
# (C) 2001 Alex Badea <vampire@go.ro>
#

# Change this to suit your system
gnuworld_home = "../"

# English language ID (or, any language ID that is known to have
# valid translated strings)
english_id = 1

# Regular expression (don't ask me how I got to it)
regstr = "(%([0123456789]+\\$)?[-\\+ #'0]*[0123456789]*(\\.[0123456789]+)?([hjlLqtzZ]|hh|ll)?[diouxXfeEgGaAcspnm])"

# ---------------------------------------------------------------------------

import sys
import os
import re
from string import *

trans = {}

def read_language_table(fname):
	print "* Reading translations from %s" % fname
	f = open(fname, "r")
	for line in f.readlines():
		line = strip(line)
		if len(line) < 2: continue
		if line[0][:1] == "--": continue
		if line[0][:1] == "\.": continue

		toks = split(line, '\t')
		if len(toks) < 4: continue
		if toks[0] == "DELETE": continue
		if toks[0] == "COPY": continue

		try:
			pk = (int(toks[0]), int(toks[1]))
		except ValueError:
			print "* Non-numeric IDs, line: %s" % `toks`
			continue
		
		if toks[len(toks)-2] != "31337":
			print "* Translation ID (%d,%d) doesn't have a '31337' timestamp" % pk
		if toks[len(toks)-1] != "0":
			print "* Translation ID (%d,%d) doesn't have a '0' deleted flag" % pk
		if trans.has_key(pk):
			print "* Duplicate translation ID (%d,%d) Text: '%s' conflicts with '%s'" % (pk[0], pk[1], toks[2], trans[toks[1]])
		trans[pk] = toks[2]
	f.close()
	print "* Read %d translations" % len(trans)

read_language_table(os.path.join(gnuworld_home, "doc", "language_table.sql"))

r = re.compile(regstr)
print "* Checking varargs..."
for (lang, tr) in trans.keys():
	if lang == english_id: continue

	ctext = trans[(lang, tr)]
	cva_list = r.findall(ctext)
	if not len(cva_list):
		if count(ctext, '%'): print '* Possible missed vararg:', ctext
	etext = trans[(english_id, tr)]
	eva_list = r.findall(etext)
	
	if cva_list != eva_list:
		print "* Mismatched vararg list, translation ID %d:" % tr
		print "  %d> %s" % (lang, ctext)
		print "  %d> %s" % (english_id, etext)

print "* Done"
