#!/usr/bin/env python
#
# Script to check GNUWorld's mod.cservice translations
# (C) 2001 Alex Badea <vampire@go.ro>
#

# Change this to suit your system
gnuworld_home = "../"

# Language ID to check
language_id = "8"

# The maximum distance, in lines, between a bot->Notice and a getResponse
notice_thresh = 1

# ---------------------------------------------------------------------------

import sys
import os
from string import *

responses = {}
resp_used = {}
trans = {}
trans_r = {}
alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"

def read_language_table(fname):
	f = open(fname, "r")
	for line in f.readlines():
		line = strip(line)
		toks = split(line, '\t')
		if len(toks) < 4: continue
		if toks[0] != language_id: continue
		if toks[len(toks)-1] != "31337": continue
		if trans.has_key(toks[1]):
			print "* Duplicate translation ID (%s) Text: '%s' conflicts with '%s' defined by ID %d" % (toks[1], toks[2], trans[toks[1]], trans_r[toks[2]])
		if trans_r.has_key(toks[2]):
			print "* Duplicate translation text '%s', ID %s already defined by ID %s" % (toks[2], toks[1], trans_r[toks[2]])
		trans[toks[1]] = toks[2]
		trans_r[toks[2]] = toks[1]
	f.close()
	print "* Read %d translations from %s" % (len(trans), fname)

def read_response_table(fname):
	f = open(fname, "r")
	for line in f.readlines():
		line = strip(line)
		toks = split(line)
		if len(toks) < 2: continue
		if toks[0] != "const": continue
		if toks[1] != "int": continue
		if responses.has_key(toks[2]):
			print "* Duplicate response name (%s) ID: %s" % (toks[2], toks[4])
		responses[toks[2]] = toks[4][:-1]
		resp_used[toks[2]] = 0
	f.close()
	print "* Read %d responses from %s" % (len(responses), fname)

def parse_file(fname):
	f = open(os.path.join(gnuworld_home, "mod.cservice", fname), "r")
	line = f.readline()
	lineno = 1
	last_notice = 0
	while line:
		# Check the integrity of language:: references
		try: start = index(line, "language::")
		except ValueError: start = -1
		if start >= 0:
			start = start + 10
			end = start
			while (line[end] in alpha) and (end < len(line)):
				end = end + 1
			id = line[start:end]
			numeric = 'UNDEFINED'
			text = '[UNASSIGNED]'
			if responses.has_key(id):
				numeric = responses[id]
				if trans.has_key(numeric):
					text = trans[numeric]
				resp_used[id] = 1
			print "%s(%d): language::%s (%s) = '%s'" % (fname, lineno, id, numeric, text)
			#print "%-25s (%4d): %-30s (%4s) -> %s" % (fname, lineno, id, numeric, text)

		# Check for unlisted translations
		try: start = index(line, "Notice")
		except ValueError: start = -1
		notice_done = 0
		if (start >= 0) and (start == 0 or not line[start-1] in alpha):
			last_notice = lineno
			notice_line = strip(line[:-1])
			if line[-2] == ";": notice_done = 1

		try: start = index(line, "getResponse")
		except ValueError: start = -1
		if start >= 0:
			last_notice = 0

		if last_notice and (lineno > last_notice + notice_thresh or notice_done):
			print "* Possible untranslated string in %s(%d): %s" % (fname, last_notice, notice_line)
			last_notice = 0


		lineno = lineno + 1
		line = f.readline()
	f.close()

read_language_table(os.path.join(gnuworld_home, "doc", "language_table.sql"))
read_response_table(os.path.join(gnuworld_home, "mod.cservice", "responses.h"))

files = os.listdir(os.path.join(gnuworld_home, "mod.cservice"))
for file in files:
	if len(file) < 4: continue
	if file[-3:] != ".cc": continue
	parse_file(file)

for resp in resp_used.keys():
	if not resp_used[resp]:
		numeric = 'UNDEFINED'
		text = '[UNASSIGNED]'
		if responses.has_key(resp):
			numeric = responses[resp]
			if trans.has_key(numeric):
				text = trans[numeric]
		print "* Unused response language::%s (%s) = '%s'" % (resp, numeric, text)

print "* Done"
