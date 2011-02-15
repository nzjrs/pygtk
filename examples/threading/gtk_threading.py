#!/usr/bin/env python


import time
import threading

import pygtk
pygtk.require('2.0')
import gtk
import gobject


def identify(message):
    t = threading.current_thread()
    print t.name, message


class WorkerThread(threading.Thread):
    def __init__(self, window):
        identify('Initializing new thread')

        threading.Thread.__init__(self)
        self.window = window

    def run(self):
        identify('Doing work in new thread')

        for i in range(10):
            # Let's do some hard work here
            time.sleep(0.5)

            # let the window know we did something
            gobject.idle_add(self.window.worker_update, 'Doing hard work: %s' % str(i+1))

            # and update the progressbar
            fraction = (i + 1) / float(10)
            gobject.idle_add(self.window.progress.set_fraction, fraction)

        # We're done
        gobject.idle_add(self.window.worker_join)


class TestWindow(gtk.Window):
    __gtype_name__ = 'TestWindow'

    def __init__(self):
        gtk.Window.__init__(self)
        self.set_border_width(4)
        self.connect('delete-event', self.on_delete_event)
        self.working = False

        self.vbox = gtk.VBox(spacing=4)
        self.add(self.vbox)

        self.progress = gtk.ProgressBar()
        self.vbox.pack_start(self.progress)

        self.label = gtk.Label()
        self.vbox.pack_start(self.label)

        self.button = gtk.Button('start thread')
        self.button.connect('clicked', self.button_clicked)
        self.vbox.pack_start(self.button)

    def on_delete_event(self, widget, event):
        if self.working:
            # do not close this window now, we'e working!
            return True
        else:
            return False

    def button_clicked(self, button):
        self.button.set_sensitive(False)
        self.worker_start()

    def worker_start(self):
        self.working = True
        self.worker = WorkerThread(self)
        self.worker.start()

    def worker_update(self, value):
        t = threading.current_thread()
        print t.name, 'Updating label'
        self.label.set_text(value)

    def worker_join(self):
        self.worker.join()
        self.button.set_sensitive(True)
        self.label.set_text('Done!')
        self.working = False


if __name__ == '__main__':
    gobject.threads_init()

    testwindow = TestWindow()
    testwindow.connect('delete-event', gtk.main_quit)
    testwindow.show_all()

    gtk.main()


