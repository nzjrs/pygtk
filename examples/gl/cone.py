import gtk
import gtkgl

from OpenGL.GL import *
from OpenGL.GLU import *

rotx = 250
roty = 0

def hchanged(hadj):
	global roty
	roty = hadj.value
	redraw(glarea)

def vchanged(vadj):
	global rotx
	rotx = vadj.value
	redraw(glarea)

quadObj = gluNewQuadric()

def glutSolidCone(base, height, slices, stacks):
	gluQuadricDrawStyle(quadObj, GLU_FILL)
	gluQuadricNormals(quadObj, GLU_SMOOTH)
	gluCylinder(quadObj, base, 0.0, height, slices, stacks)

def redraw(glarea, event=None):
	if not glarea.make_current(): return
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
	glPushMatrix()
	glScale(0.5, 0.5, 0.5)
	#glTranslate(0, -1, 0)
	glRotatef(rotx, 1, 0, 0)
	glRotatef(roty, 0, 1, 0)
	glutSolidCone(1, 2, 50, 10)
	glPopMatrix()
	glarea.swap_buffers()

def realise(glarea):
	if glarea.make_current():
		glMaterial(GL_FRONT, GL_AMBIENT, [0.2, 0.2, 0.2, 1.0])
		glMaterial(GL_FRONT, GL_DIFFUSE, [0.8, 0.8, 0.8, 1.0])
		glMaterial(GL_FRONT, GL_SPECULAR, [1.0, 0.0, 1.0, 1.0])
		glMaterial(GL_FRONT, GL_SHININESS, 50.0)
		glLight(GL_LIGHT0, GL_AMBIENT, [0.0, 1.0, 0.0, 1.0])
		glLight(GL_LIGHT0, GL_DIFFUSE, [1.0, 1.0, 1.0, 1.0])
		glLight(GL_LIGHT0, GL_SPECULAR, [1.0, 1.0, 1.0, 1.0])
		glLight(GL_LIGHT0, GL_POSITION, [1.0, 1.0, 1.0, 0.0]);   
		glLightModel(GL_LIGHT_MODEL_AMBIENT, [0.2, 0.2, 0.2, 1.0])
		glEnable(GL_LIGHTING)
		glEnable(GL_LIGHT0)
		glDepthFunc(GL_LESS)
		glEnable(GL_DEPTH_TEST)

if not gtkgl.query():
	print "OpenGL not supported.  Bye."
	raise SystemExit

win = gtk.GtkWindow()
win.connect("destroy", gtk.mainquit)
win.set_title("Cone")

table = gtk.GtkTable(2, 3)
table.set_border_width(5)
table.set_col_spacings(5)
table.set_row_spacings(5)
win.add(table)
table.show()

vadj = gtk.GtkAdjustment(250, 0, 360, 5, 5, 0)
vscale = gtk.GtkVScale(vadj)
table.attach(vscale, 1,2, 0,1, xoptions=gtk.FILL)
vscale.show()

hadj = gtk.GtkAdjustment(0, 0, 360, 5, 5, 0)
hscale = gtk.GtkHScale(hadj)
table.attach(hscale, 0,1, 1,2, yoptions=gtk.FILL)
hscale.show()

vadj.connect("value_changed", vchanged)
hadj.connect("value_changed", hchanged)

glarea = gtkgl.GtkGLArea((gtkgl.RGBA, gtkgl.DOUBLEBUFFER, gtkgl.DEPTH_SIZE, 1))
glarea.size(300, 300)

glarea.connect("realize", realise)
glarea.connect("expose_event", redraw)

table.attach(glarea, 0,1, 0,1)
glarea.show()

win.show()

gtk.mainloop()
glarea.destroy()
