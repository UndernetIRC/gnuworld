#!/usr/bin/env python
#
# Script to check GNUWorld's mod.cservice translations
# Episode 2 - Check for inconsistent translation strings
#
# (C) 2001 Alex Badea <vampire@go.ro>
#

# Change this to suit your system
gnuworld_home = "../"

# Language ID to check
language_id = "1"

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


class Parser:
	def __init__(self, fname):
		self.fname = fname
		f = open(os.path.join(gnuworld_home, "mod.cservice", fname), "r")
		self.buf = f.read()
		f.close()
	def pos(self, sub, start=0):
		try: return index(self.buf, sub, start)
		except ValueError: return -1
	def run(self):
		idx = self.pos("getResponse", 0)
		while idx != -1:
			lineno = count(self.buf[:idx], '\n') + 1
			idx = idx + len("getResponse")
			while self.buf[idx] != "(":
				idx = idx + 1
			idx = idx + 1
			
			para = 1
			quote = 0
			tok = ""
			tokens = []
			while 1:
				ch = self.buf[idx]
				if quote:
					if ch == '"':
						tok = replace(tok, '""', '')
						quote = 0
				else:
					if ch == "(":
						para = para + 1
					elif ch == ")":
						para = para - 1
						if para == 0:
							tokens.append(tok)
							break
					elif ch == "\"":
						quote = 1
					elif ch in " \n\t\r":
						idx = idx + 1
						continue
					elif ch == ",":
						tokens.append(tok)
						tok = ""
						idx = idx + 1
						continue
					elif ch == ";":
						tokens.append(tok)
						break
				tok = tok + ch
				idx = idx + 1
			if len(tokens) < 3:
				print "%s(%d): getResponse has less than 3 arguments" % (self.fname, lineno)
				pass
			elif len(tokens[1]) < 10 or tokens[1][:10] != "language::":
				print "%s(%d): Second argument to getResponse doesn't start with 'language::'" % (self.fname, lineno)
			else:
				id = tokens[1][10:]
				numeric = responses[id]
				trans_text = trans[numeric]
				text = tokens[2]
				if text[:7] == "string(": text = text[7:]
				if text[-1] == ")": text = text[:-1]
				if text[0] == '"': text = text[1:]
				if text[-1] == '"': text = text[:-1]
				if len(tokens) > 2 and trans_text != text:
					print "%s(%d): Default response doesn't match with translation:\n    Def:   %s\n    Trans: %s" % (self.fname, lineno, text, trans_text)

			idx = self.pos("getResponse", idx)
			
read_language_table(os.path.join(gnuworld_home, "doc", "language_table.sql"))
read_response_table(os.path.join(gnuworld_home, "mod.cservice", "responses.h"))

files = os.listdir(os.path.join(gnuworld_home, "mod.cservice"))
for file in files:
	if len(file) < 4: continue
	if file[-3:] != ".cc": continue
	Parser(file).run()

print "* Done"
