# order is important here (for now)
import gobject, gtk

# string taken from pango examples directory and converted from utf8
# to python unicode string escapes
hellos = u'''\u03A0\u03B1\u03BD\u8A9E
This is a list of ways to say hello in various languages. 
The purpose of it is to illustrate a number of scripts.

(Converted into UTF-8)

---------------------------------------------------------
Arabic\t\u0627\u0644\u0633\u0644\u0627\u0645 \u0639\u0644\u064A\u0643\u0645
Czech\t(\u010Desky)\tDobr\375 den
Danish\t(Dansk)\tHej, Goddag
English\tHello
Esperanto\tSaluton
Estonian\tTere, Tervist
FORTRAN\tPROGRAM
Finnish\t(Suomi)\tHei
French\t(Fran\347ais)\tBonjour, Salut
German\t(Deutsch Nord)\tGuten Tag
German\t(Deutsch S\374d)\tGr\374\337 Gott
Greek\t(\u0395\u03BB\u03BB\u03B7\u03BD\u03B9\u03BA\u03AC)\t\u0393\u03B5\u03B9\u03AC \u03C3\u03B1\u03C2
Hebrew\t\u05E9\u05DC\u05D5\u05DD
Hindi\t\u0928\u092E\u0938\u094D\u0924\u0947, \u0928\u092E\u0938\u094D\u0915\u093E\u0930\u0964
Italiano\tCiao, Buon giorno
Maltese\t\u010Aaw, Sa\u0127\u0127a
Nederlands, Vlaams\tHallo, Dag
Norwegian\t(Norsk)\tHei, God dag
Polish\tDzie\u0144 dobry, Hej
Russian\t(\u0420\u0443\u0441\u0441\u043A\u0438\u0439)\t\u0417\u0434\u0440\u0430\u0432\u0441\u0442\u0432\u0443\u0439\u0442\u0435!\u200E
Slovak\tDobr\375 de\u0148
Spanish\t(Espa\361ol)\t\u200E\241Hola!\u200E
Swedish\t(Svenska)\tHej, Goddag
Thai\t(\u0E20\u0E32\u0E29\u0E32\u0E44\u0E17\u0E22)\t\u0E2A\u0E27\u0E31\u0E2A\u0E14\u0E35\u0E04\u0E23\u0E31\u0E1A, \u0E2A\u0E27\u0E31\u0E2A\u0E14\u0E35\u0E04\u0E48\u0E30
Turkish\t(T\374rk\347e)\tMerhaba
Vietnamese\t(Ti\u1EBFng Vi\u1EC7t)\tXin Ch\340o
Yiddish\t(\u05F2\u05B7\u05D3\u05D9\u05E9\u05E2)\t\u05D3\u05D0\u05B8\u05E1 \u05D4\u05F2\u05B7\u05D6\u05E2\u05DC\u05E2

Japanese\t(\u65E5\u672C\u8A9E)\t\u3053\u3093\u306B\u3061\u306F, \uFF7A\uFF9D\uFF86\uFF81\uFF8A
Chinese\t(\u4E2D\u6587,\u666E\u901A\u8BDD,\u6C49\u8BED)\t\u4F60\u597D
Cantonese\t(\u7CB5\u8A9E,\u5EE3\u6771\u8A71)\t\u65E9\u6668, \u4F60\u597D
Korean\t(\uD55C\uAE00)\t\uC548\uB155\uD558\uC138\uC694, \uC548\uB155\uD558\uC2ED\uB2C8\uAE4C

Difference among chinese characters in GB, JIS, KSC, BIG5:\u200E
 GB\t--\t\u5143\u6C14\t\u5F00\u53D1
 JIS\t--\t\u5143\u6C17\t\u958B\u767A
 KSC\t--\t\u5143\u6C23\t\u958B\u767C
 BIG5\t--\t\u5143\u6C23\t\u958B\u767C

'''
win = gtk.Window()
win.connect('destroy', gtk.mainquit)
win.set_default_size(600, 400)

swin = gtk.ScrolledWindow()
swin.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
win.add(swin)
swin.show()

l = gtk.Label(hellos)
swin.add_with_viewport(l)
l.show()

win.show()

gtk.main()

