#!/usr/bin/env python

# A translation of the gears demo that comes with mesa, modified to use
# a few GtkHScale widgets for the rotation, rather than the keyboard.

from OpenGL.GL import *
import gtk
import gtkgl

import math

# Draw a gear wheel.  You'll probably want to call this function when
# building a display list since we do a lot of trig here.
#
# Input:  inner_radius - radius of hole at center
#         outer_radius - radius at center of teeth
#         width - width of gear
#         teeth - number of teeth
#         tooth_depth - depth of tooth
def gear(inner_radius, outer_radius, width, teeth, tooth_depth):
	cos = math.cos
	sin = math.sin
	
	r0 = inner_radius
	r1 = outer_radius - tooth_depth/2.0
	r2 = outer_radius + tooth_depth/2.0

	da = 2.0*math.pi / teeth / 4.0

	glShadeModel( GL_FLAT )

	glNormal3f( 0.0, 0.0, 1.0 )

	# draw front face 
	glBegin( GL_QUAD_STRIP )
	for i in range(teeth + 1):
		angle = i * 2.0*math.pi / teeth
		glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 )
		glVertex3f( r1*cos(angle), r1*sin(angle), width*0.5 )
		glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 )
		glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 )
	glEnd();

	# draw front sides of teeth
	glBegin( GL_QUADS )
	da = 2.0*math.pi / teeth / 4.0
	for i in range(teeth):
		angle = i * 2.0*math.pi / teeth
		glVertex3f( r1*cos(angle),      r1*sin(angle),      width*0.5 )
		glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),   width*0.5 )
		glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), width*0.5 )
		glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 )
	glEnd()


	glNormal3f( 0.0, 0.0, -1.0 )

	# draw back face
	glBegin( GL_QUAD_STRIP )
	for i in range(teeth + 1):
		angle = i * 2.0*math.pi / teeth
		glVertex3f( r1*cos(angle), r1*sin(angle), -width*0.5 )
		glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 )
		glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da),-width*0.5 )
		glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 )
	glEnd()

	# draw back sides of teeth
	glBegin( GL_QUADS )
	da = 2.0*math.pi / teeth / 4.0
	for i in range(teeth):
		angle = i * 2.0*math.pi / teeth

		glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da),-width*0.5 )
		glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da),-width*0.5 )
		glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),  -width*0.5 )
		glVertex3f( r1*cos(angle),      r1*sin(angle),     -width*0.5 )
	glEnd()


	# draw outward faces of teeth
	glBegin( GL_QUAD_STRIP );
	for i in range(teeth):
		angle = i * 2.0*math.pi / teeth

		glVertex3f( r1*cos(angle), r1*sin(angle),  width*0.5 )
		glVertex3f( r1*cos(angle), r1*sin(angle), -width*0.5 )
		u = r2*cos(angle+da) - r1*cos(angle)
		v = r2*sin(angle+da) - r1*sin(angle)
		len = math.sqrt( u*u + v*v )
		u = u / len
		v = v / len
		glNormal3f( v, -u, 0.0 )
		glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),   width*0.5 )
		glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),  -width*0.5 )
		glNormal3f( cos(angle), sin(angle), 0.0 )
		glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), width*0.5 )
		glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da),-width*0.5 )
		u = r1*cos(angle+3*da) - r2*cos(angle+2*da)
		v = r1*sin(angle+3*da) - r2*sin(angle+2*da)
		glNormal3f( v, -u, 0.0 )
		glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 )
		glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da),-width*0.5 )
		glNormal3f( cos(angle), sin(angle), 0.0 )

	glVertex3f( r1*cos(0), r1*sin(0), width*0.5 )
	glVertex3f( r1*cos(0), r1*sin(0), -width*0.5 )

	glEnd()


	glShadeModel( GL_SMOOTH )

	# draw inside radius cylinder
	glBegin( GL_QUAD_STRIP )
	for i in range(teeth + 1):
		angle = i * 2.0*math.pi / teeth;
		glNormal3f( -cos(angle), -sin(angle), 0.0 )
		glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 )
		glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 )
	glEnd()

view_rotx=20.0
view_roty=30.0
view_rotz=0.0

gear1 = 0
gear2 = 0
gear3 = 0

angle = 0.0

def xchange(adj):
	global view_rotx
	view_rotx = adj.value
	draw(glarea)
def ychange(adj):
	global view_roty
	view_roty = adj.value
	draw(glarea)
def zchange(adj):
	global view_rotz
	view_rotz = adj.value
	draw(glarea)

def draw(glarea, event=None):
	if not glarea.make_current(): return
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT )

	glPushMatrix()
	glRotatef( view_rotx, 1.0, 0.0, 0.0 )
	glRotatef( view_roty, 0.0, 1.0, 0.0 )
	glRotatef( view_rotz, 0.0, 0.0, 1.0 )

	glPushMatrix()
	glTranslatef( -3.0, -2.0, 0.0 )
	glRotatef( angle, 0.0, 0.0, 1.0 )
	glCallList(gear1)
	glPopMatrix()

	glPushMatrix()
	glTranslatef( 3.1, -2.0, 0.0 )
	glRotatef( -2.0*angle-9.0, 0.0, 0.0, 1.0 )
	glCallList(gear2)
	glPopMatrix()

	glPushMatrix()
	glTranslatef( -3.1, 4.2, 0.0 )
	glRotatef( -2.0*angle-25.0, 0.0, 0.0, 1.0 )
	glCallList(gear3)
	glPopMatrix()

	glPopMatrix()

	glarea.swap_buffers()

def idle():
	global angle
	angle = angle + 2.0

	draw(glarea)

	return gtk.TRUE

def reshape_idle():
	if not glarea.make_current(): return
	x, y, width, height = glarea.get_allocation()

	glViewport(0, 0, width, height)
	glMatrixMode(GL_PROJECTION)
	glLoadIdentity()
	if width > height:
		w = width / height
		glFrustum( -w, w, -1.0, 1.0, 5.0, 60.0 )
	else:
		h = height / width
		glFrustum( -1.0, 1.0, -h, h, 5.0, 60.0 )

	glMatrixMode(GL_MODELVIEW)
	glLoadIdentity()
	glTranslatef( 0.0, 0.0, -40.0 )
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT )

	return gtk.FALSE

def reshape(glarea, allocation):
	gtk.idle_add(reshape_idle)

def init(glarea):
	if not glarea.make_current(): return
	
	global gear1, gear2, gear3

	pos = (5.0, 5.0, 10.0, 0.0 )
	red = (0.8, 0.1, 0.0, 1.0 )
	green = (0.0, 0.8, 0.2, 1.0 )
	blue = (0.2, 0.2, 1.0, 1.0 )

	glLightfv( GL_LIGHT0, GL_POSITION, pos )
	glEnable( GL_CULL_FACE )
	glEnable( GL_LIGHTING )
	glEnable( GL_LIGHT0 )
	glEnable( GL_DEPTH_TEST )

	# make the gears
	gear1 = glGenLists(1)
	glNewList(gear1, GL_COMPILE)
	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red )
	gear( 1.0, 4.0, 1.0, 20, 0.7 )
	glEndList()

	gear2 = glGenLists(1)
	glNewList(gear2, GL_COMPILE)
	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green )
	gear( 0.5, 2.0, 2.0, 10, 0.7 )
	glEndList()

	gear3 = glGenLists(1)
	glNewList(gear3, GL_COMPILE)
	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue )
	gear( 1.3, 2.0, 0.5, 10, 0.7 )
	glEndList()

	glEnable( GL_NORMALIZE )


if not gtkgl.query():
	print "OpenGL not supported.  Bye."
	raise SystemExit

win = gtk.GtkWindow()
win.connect("destroy", gtk.mainquit)
win.set_title("Gears")

table = gtk.GtkTable(4, 2)
win.add(table)
table.show()

glarea = gtkgl.GtkGLArea((gtkgl.RGBA, gtkgl.DEPTH_SIZE, 1, gtkgl.DOUBLEBUFFER))
glarea.size(300, 300)

glarea.connect("realize", init)
glarea.connect("size_allocate", reshape)
glarea.connect("expose_event", draw)

gtk.idle_add(idle)

table.attach(glarea, 0,2, 0,1)
glarea.show()

for row, label, start, cb in ((1,'X Rotation', view_rotx, xchange),
			      (2,'Y Rotation', view_roty, ychange),
			      (3,'Z Rotation', view_rotz, zchange)):
	l = gtk.GtkLabel(label)
	table.attach(l, 0,1, row,row+1, xoptions=0, yoptions=gtk.FILL)
	l.show()

	adj = gtk.GtkAdjustment(start, 0, 360, 5, 5, 0)
	adj.connect("value_changed", cb)

	scale = gtk.GtkHScale(adj)
	table.attach(scale, 1,2, row,row+1, yoptions=gtk.FILL)
	scale.show()

win.show()

gtk.mainloop()

glarea.destroy()
