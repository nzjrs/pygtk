#!/usr/bin/env python
try:
	from Numeric import *
except:
	print "This demo requires the Numeric extension, sorry"
	import sys
	sys.exit()
from RandomArray import *
import string, sys
from OpenGL.GL import *

import gtk
import gtkgl

MY_LIST=1
NUMDOTS = 500
NUMDOTS2 = 600
x = random(NUMDOTS)*2-1
y = random(NUMDOTS)*2-1
MAX_AGE = 13
age = randint(0,MAX_AGE,(NUMDOTS,))
move_length = .005  # 1.0 = screen width
angle = 0          # in radians
delta_angle = .2  # in radians
move_x = move_length*cos(angle)
move_y = move_length*sin(angle)

glarea = None
timer_id = 0

def display(area=None, event=None):
	global x, y, move_x, move_y, NUMDOTS, NUMDOTS2, MAX_AGE, age

	# do not draw unless we can make the area the current drawable
	if not glarea.make_current(): return

	glClearColor(0.0, 0.0, 0.0, 0.0)
	glClear(GL_COLOR_BUFFER_BIT)
	glColor3f(1.0,1.0,0.0)
	glBegin(GL_POINTS)
	x = x + move_x
	y = y + move_y
	age = age + 1
	which = greater(age, MAX_AGE)
	x = choose(which, (x, random(NUMDOTS)))
	y = choose(which, (y, random(NUMDOTS)))
	age = choose(which, (age, 0))
	x = choose(greater(x,1.0),(x,x-1.0))  # very cool - wraparound
	y = choose(greater(y,1.0),(y,y-1.0))
	glVertex(transpose(array([x,y])))
	glVertex(transpose(array([x-.005,y+.005])))
	glVertex(transpose(array([x+.005,y-.005])))
	x2 = random(NUMDOTS2)
	y2 = random(NUMDOTS2)
	glVertex(transpose(array([x2,y2])))
	glEnd()
	glFlush()
	
	glarea.swap_buffers()

	return gtk.TRUE

def mouse(glarea, event):
	global angle, delta_angle, move_x, move_y, move_length, timer_id

	if event.button == 1:
		angle = angle + delta_angle
	elif event.button == 3:
		angle = angle - delta_angle
	elif event.button == 2:
		if timer_id:
			gtk.timeout_remove(timer_id)
			timer_id = 0
		else:
			timer_id = gtk.idle_add(display)
	move_x = move_length * cos(angle)
	move_y = move_length * sin(angle)

def setup_viewport():
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0)

def reshape_callback():
	x, y, w, h = glarea.get_allocation()

	glViewport(0, 0, w, h)
	setup_viewport()
	return gtk.FALSE

def reshape(glarea, allocation):
	# the allocation parameter here is of type GTK_TYPE_POINTER, so we
	# can't get to it directly -- so we set an idle callback and use
	# the get_allocation method
	gtk.idle_add(reshape_callback)

def main():
	global glarea, timer_id
	win = gtk.GtkWindow()
	win.set_title("GL Dots")
	win.connect("destroy", gtk.mainquit)
	
	glarea = gtkgl.GtkGLArea((gtkgl.RGBA, gtkgl.DOUBLEBUFFER))
	glarea.add_events(gtk.GDK.BUTTON_PRESS_MASK)
	glarea.size(300, 300)

	setup_viewport()
	glarea.connect_after("size_allocate", reshape)
	glarea.connect("expose_event", display)
	glarea.connect("button_press_event", mouse)

	timer_id = gtk.idle_add(display)

	win.add(glarea)
	glarea.show()
	win.show()

	gtk.mainloop()
	glarea.destroy()

if gtkgl.query():
	print "Use the mouse buttons to control some of the dots."

	main()
else:
	print "Your X server does not support GLX"



