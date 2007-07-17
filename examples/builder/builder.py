import gtk

class BuilderExample:
    def __init__(self, filename):
        # create a new empty Builer object
        builder = gtk.Builder()
        
        # load an XML definition
        builder.add_from_file(filename)
        
        # autoconnect the signals from the instance self
        builder.connect_signals(self)

        # get the main window
        window = builder.get_object("window1")
        window.connect("destroy", gtk.main_quit)
        window.show_all()

    def about_activate(self, action, data=None):
        about_dlg = gtk.AboutDialog()
        about_dlg.set_name("GtkBuilder demo")
        about_dlg.run()
        about_dlg.destroy()

    def quit_activate(self, action, data=None):
        gtk.main_quit()
    
    def main(self):
        gtk.main()

if __name__ == '__main__':
    build = BuilderExample("demo.ui")
    build.main()
