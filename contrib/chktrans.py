#!/usr/bin/env python

# Check GNUWorld's mod.cservice translations
# (C) 2001 Alex Badea <vampire@go.ro>

# Use the following command in the mod.cservice directory to get the grep data:
# grep -n "language::" *.cc > grep-data

import sys
from string import *

language_id = '8'

responses = {}
resp_used = {}
trans = {}
alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"

f = open("language_table.sql", "r")
for line in f.readlines():
	line = strip(line)
	toks = split(line, '\t')
	if len(toks) < 4: continue
	if toks[0] != language_id: continue
	if toks[len(toks)-1] != "31337": continue
	if trans.has_key(toks[1]):
		print "* Duplicate translation, ID: ", toks[1]
	trans[toks[1]] = toks[2]
f.close()
print "* Read %d translations from language_table.sql" % len(trans)

f = open("responses.h", "r")
for line in f.readlines():
	line = strip(line)
	toks = split(line)
	if len(toks) < 2: continue
	if toks[0] != "const": continue
	if toks[1] != "int": continue
	if responses.has_key(toks[2]):
		print "* Duplicate response, name: ", toks[2]
	responses[toks[2]] = toks[4][:-1]
	resp_used[toks[2]] = 0
f.close()
print "* Read %d responses from responses.h" % len(responses)

f = open("grep-data", "r")
line = f.readline()
while line:
	try: start = index(line, "language::")
	except ValueError: start = -1
	if start >= 0:
		start = start + 10
		end = start
		while (line[end] in alpha) and (end < len(line)):
			end = end + 1
		id = line[start:end]
		toks = split(line, ':')
		fname = ""
		fline = 0
		if len(toks) > 0: fname = toks[0]
		if len(toks) > 1: fline = int(toks[1])
		try: numeric = responses[id]
		except KeyError: numeric = '****';
		try: text = trans[numeric]
		except KeyError: text = '**UNASSIGNED**';
		try: resp_used[id] = 1
		except: pass
		print "%-25s (%4d): %-30s (%4s) -> %s" % (fname, fline, id, numeric, text)
	line = f.readline()
f.close()

for resp in resp_used.keys():
	if not resp_used[resp]:
		try: numeric = responses[resp]
		except KeyError: numeric = '****';
		try: text = trans[numeric]
		except KeyError: text = '**UNASSIGNED**';
		print "%-25s       : %-30s (%4s) -> %s" % ("* Unused response", resp, numeric, text)

print "* Done"
