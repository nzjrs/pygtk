#!/usr/bin/env python
from gtk import *

# a nice easy to read fixed spacing font.
font = load_font("-*-lucidatypewriter-medium-r-*-*-14-*-*-*-*-*-*-*")

list = "abcdefghijklmnopqrstuvwxyz" + \
       "ABCDEFGHIJKLMNOPQRSTUVWXYZ" + \
       "0123456789" + \
       "`~!@#$%^&*()-_=+\|[]{};:'\",.<>/? "
printable = ''
for c in map(chr, range(256)):
	if c in list:
		printable = printable + c
	else:
		printable = printable + '.'
del list, c

def format_line(str):
	hexstr = reduce(lambda x, a: x + hex(256+ord(a))[-2:] + ' ', str, '')
	if len(hexstr) < 48:
		hexstr = (hexstr + ' '*48)[:48]
	return hexstr + ' ' + reduce(lambda x, a: x + printable[ord(a)],str,'')

def format_data(str):
	ret = ''
	while len(str) > 16:
		line = str[:16]
		str = str[16:]
		ret = ret + format_line(line) + '\n'
	if str: ret = ret + format_line(str)
	return ret

def dnd_drop(b, event):
	data_type.set_text(event.data_type)
	data.delete_text(0, data.get_length())
	data.insert(font, black, white, format_data(event.data))
	pass

win = GtkWindow()
win.set_title("Drag to Me")
win.border_width(10)

t = GtkTable(5,4)
win.add(t)
t.show()

l = GtkLabel("Data Type")
l.set_justify(JUSTIFY_RIGHT)
t.attach(l, 0,1, 0,1, xoptions=FILL)
l.show()

data_type = GtkLabel("*None*")
data_type.set_justify(JUSTIFY_LEFT)
t.attach(data_type, 1,2, 0,1)
data_type.show()

l = GtkLabel("Data")
l.set_justify(JUSTIFY_RIGHT)
t.attach(l, 0,1, 1,2, xoptions=FILL)
l.show()

data = GtkText()
data.set_usize(600, -1)
style = data.get_style()
white = style.white
black = style.black
t.attach(data, 1,2, 1,2)
data.show()

win.connect("drop_data_available_event", dnd_drop)
win.dnd_drop_set(TRUE, ['text/plain', 'application/x-color', 'ALL'], FALSE)

win.connect("destroy", mainquit)
win.show()

mainloop()
