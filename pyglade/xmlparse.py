"""This file contains a simple parser for the XML output of GLADE.

There are two parsers in this file.  One uses xmllib, so will only be
used if xmllib is in your python module library (it is included with
python >= 1.5).  The second one is less complete, but will parse most
GLADE XML files.

The second one is not a true XML parser, since it requires tags to be
of the form:

  <tag>
    <tag2>data</tag2>
  </tag>

That is tags with embeded tags in them must have the opening tag on a single
line, and the closing tag on another line by itself.  Tags with no embedded
tags should have openning tag, data and closing tag all on one line.  Also
tag attributes are not supported.  Yes I know this is a bit lame, but it is
the minimum required for reading GLADE output.

This module is not really glade specific, except that it can read GLADE
output (it can probably also read some other types of XML documents)

You should call one of read(fname), read_stream(fp) or read_string(str).
The output is a tree of TagTree.  Tags of a node can be accessed either as
attributes (eg node.tag) or as list items (eg node['tag']).  If there was
more than one of that tag name at this level, they will be returned as a
tuple by the previous two methods.
"""

import string
import regex

# StringIO module for parsing an XML document stored in a string.  We pick
# the faster cStringIO implementation if present.
try:
	from cStringIO import StringIO
except ImportError:
	from StringIO import StringIO

error = "pyglade.xmlparse.error"

comment_line = regex.compile('<\\?.*>')
# data_tag must be checked first, since open_tag and close_tag will match
# the same strings data_tag matches.
data_tag     = regex.compile('<\([^>]+\)>\(.*\)</\([^>]+\)>')
open_tag     = regex.compile('<\([^/>][^>]*\)>')
close_tag    = regex.compile('</\([^>]+\)>')

class TagTree:
	def __init__(self, parent, tag):
		self.parent = parent
		self.tag = tag
		self.__tags = {}
	def __getitem__(self, key):
		return self.__tags[key]
	__getattr__ = __getitem__
	def __setitem__(self, key, value):
		self.__tags[key] = value
	def __len__(self):
		return len(self.__tags)
	def has_key(self, key):
		return self.__tags.has_key(key)
	def keys(self):
		return self.__tags.keys()
	def get(self, key, default):
		if self.__tags.has_key(key):
			return self.__tags[key]
		else:
			return default
	def get_bool(self, key, default=0):
		if self.__tags.has_key(key):
			return string.lower(self.__tags[key]) == 'true'
		else:
			return default
	def get_int(self, key, default=0):
		if self.__tags.has_key(key):
			return string.atoi(self.__tags[key])
		else:
			return default
	def get_float(self, key, default=0.0):
		if self.__tags.has_key(key):
			return string.atof(self.__tags[key])
		else:
			return default
	def destroy(self):
		# This is required to break a dependency loop
		del self.parent
		for key in self.__tags.keys():
			vals = self.__tags[key]
			if type(vals) != type(()): vals = (vals,)
			for val in vals:
				if hasattr(val, 'destroy'): val.destroy()
			del self.__tags[key]

def read_stream(fp):
	base = TagTree(parent=None, tag='XML-Base')
	cstack = [base]
	line = fp.readline()
	while line:
		if comment_line.search(line) >= 0 or line == '\n':
			pass
		elif data_tag.search(line) >= 0:
			key = string.lower(data_tag.group(1))
			data = data_tag.group(2)
			end = string.lower(data_tag.group(3))
			if key != end:
				raise error, "unmatching tags: %s and %s" % \
				      (key, end)
			if cstack[-1].has_key(key):
				oldval = cstack[-1][key]
				if type(oldval) == type(()):
					cstack[-1][key] = oldval + (data,)
				else:
					cstack[-1][key] = (oldval, data)
			else:
				cstack[-1][key] = data
		elif open_tag.search(line) >= 0:
			key = string.lower(open_tag.group(1))
			tree = TagTree(parent=cstack[-1], tag=key)
			if cstack[-1].has_key(key):
				oldval = cstack[-1][key]
				if type(oldval) == type(()):
					cstack[-1][key] = oldval + (tree,)
				else:
					cstack[-1][key] = (oldval, tree)
			else:
				cstack[-1][key] = tree
			cstack.append(tree)
		elif close_tag.search(line) >= 0:
			key = string.lower(close_tag.group(1))
			if not cstack:
				raise error, "no tags to match " + key
			if key != cstack[-1].tag:
				raise error, \
				      "unmatching container tags: %s and %s" %\
				      (cstack[-1].type, key)
			del cstack[-1]
		else:
			raise error, "unparseable line: " + line
		line = fp.readline()
	if len(cstack) != 1:
		raise error, "some unclosed tags are present"
	return base

def read(fname):
	return read_stream(open(fname, "r"))

def read_string(string):
	return read_stream(StringIO(string))

try:
	# this is a better implementation that uses the xmllib library.
	import xmllib
	class myParser(xmllib.XMLParser):
		def __init__(self):
			xmllib.XMLParser.__init__(self)
			self.base = TagTree(parent=None, tag='XML-Base')
			self.cstack = [self.base]
			self.curName = None
			self.curData = ""
		def unknown_starttag(self, tag, attrs):
			tag = string.lower(tag)
			if self.curName:
				cur = self.curName
				tree = TagTree(parent=self.cstack[-1],
					       tag=cur)
				if self.cstack[-1].has_key(cur):
					oldval = self.cstack[-1][cur]
					if type(oldval) == type(()):
						self.cstack[-1][cur] = \
							oldval + (tree,)
					else:
						self.cstack[-1][cur] = \
							(oldval,tree)
				else:
					self.cstack[-1][cur] = tree
				self.cstack.append(tree)
				# for glade files, tags with child tags don't
				# have any data associated with them
				self.curData = ""
			self.curName = tag
		def unknown_endtag(self, tag):
			tag = string.lower(tag)
			self.curData = string.strip(self.curData)
			if self.curName:
				if self.curName != tag:
					raise error, \
					      "unmatching tags: %s and %s" % \
					      (self.curName, tag)
				if self.cstack[-1].has_key(tag):
					oldval = self.cstack[-1][tag]
					if type(oldval) == type(()):
						self.cstack[-1][tag] = \
							oldval +(self.curData,)
					else:
						self.cstack[-1][tag] = \
							(oldval,self.curData)
				else:
					self.cstack[-1][tag] = self.curData
			else:
				if not self.cstack:
					raise error, "no tags to match " + tag
				if self.cstack[-1].tag != tag:
					raise error, \
					      "unmatching tags: %s and %s" % \
					      (self.cstack[-1].tag, tag)
				del self.cstack[-1]
			self.curName = None
			self.curData = ""
		def handle_data(self, data):
			self.curData = self.curData + data

	def read_string(string):
		parser = myParser()
		parser.feed(string)
		if len(parser.cstack) != 1:
			raise error, "some unclosed tags are present"
		return parser.base

	def read_stream(fp):
		parser = myParser()
		data = fp.read(8192)
		while data:
			parser.feed(data)
			data = fp.read(8192)
		if len(parser.cstack) != 1:
			raise error, "some unclosed tags are present"
		return parser.base

	def read(fname):
		return read_stream(open(fname, "r"))

except ImportError:
	pass
	
