#
# Small example of the new GtkUIManager
#
# Johan Dahlin <johan@gnome.org>, 2004
#

import pygtk
pygtk.require('2.0')

import gtk

if gtk.pygtk_version > (2,3,90):
    print "PyGtk 2.3.90 or later required for this example"
    raise SystemExit

window_actions = [
    ('FileMenu', None, '_File'),
    ('New',      gtk.STOCK_NEW, '_New', '<control>N', 'Create a new file', 'file_new_cb'),
    ('Open',     gtk.STOCK_OPEN, '_Open', '<control>O', 'Open a file', 'file_open_cb'),
    ('Quit',     gtk.STOCK_QUIT, '_Quit', '<control>Q', 'Quit application', 'file_quit_cb'),
    ('HelpMenu', None, '_Help'),
    ('About',    None, '_About', None, 'About application', 'help_about_cb'),
    ]

ui_string = """<ui>
  <menubar name='MenuBar'>
    <menu action='FileMenu'>
      <menuitem action='New'/>
      <menuitem action='Open'/>
      <separator/>
      <menuitem action='Quit'/>
    </menu>
    <menu action='HelpMenu'>
      <menuitem action='About'/>
    </menu>
  </menubar>
</ui>"""

def fix_actions(actions, instance):
    "Helper function to map methods to an instance"
    retval = []
    
    for i in range(len(actions)):
        curr = actions[i]
        if len(curr) > 5:
            curr = list(curr)
            curr[5] = getattr(instance, curr[5])
            curr = tuple(curr)
            
        retval.append(curr)
    return retval

class Window(gtk.Window):
    def __init__(self):
        gtk.Window.__init__(self)
        self.connect('delete-event', self.delete_event_cb)
        self.set_size_request(400, 200)
        vbox = gtk.VBox()
        self.add(vbox)

        menu = self.create_ui()
        vbox.pack_start(menu, expand=False)

        sw = gtk.ScrolledWindow()
        vbox.pack_start(sw)

        textview = gtk.TextView()
        self.buffer = textview.get_buffer()
        sw.add(textview)
        
        status = gtk.Statusbar()
        vbox.pack_end(status, expand=False)
        
    def create_ui(self):
        ag = gtk.ActionGroup('WindowActions')

        actions = fix_actions(window_actions, self)
        ag.add_actions(actions)
        self.ui = gtk.UIManager()
        self.ui.insert_action_group(ag, 0)
        self.ui.add_ui_from_string(ui_string)
        menu = self.ui.get_widget('/MenuBar')
        return menu

    def file_new_cb(self, action):
        self.buffer.set_text("")
        
    def file_open_cb(self, action):
        dialog = gtk.FileChooserDialog("Open..",
                                       None,
                                       gtk.FILE_CHOOSER_ACTION_OPEN,
                                       (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                        gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        dialog.set_default_response(gtk.RESPONSE_OK)

        filter = gtk.FileFilter()
        filter.set_name("All files")
        filter.add_pattern("*")
        dialog.add_filter(filter)

        if dialog.run() == gtk.RESPONSE_OK:
            dialog.hide()
            filename = dialog.get_filename()
            self.buffer.set_text(file(filename).read())

    def file_quit_cb(self, action):
        self.close()

    def help_about_cb(self, action):
        dialog = gtk.MessageDialog(self,
                                   gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT,
                                   gtk.MESSAGE_INFO, gtk.BUTTONS_OK,
                                   "Small example of the new GtkUIManger class")
        dialog.run()
        dialog.destroy()

    def delete_event_cb(self, window, event):
        self.close()

    def close(self):
        gtk.main_quit()

if __name__ == '__main__':
    w = Window()
    w.show_all()
    gtk.main()
