
import os

_modlist = filter(lambda x: len(x) > 3 and x[-3:] == '.py',
		  os.listdir(os.path.dirname(__file__)))

demos = []
for _mod in _modlist:
    if _mod[0] != '_':
	_mod = _mod[:-3]
	try:
	    exec 'import ' + _mod + '\n' + \
		 '_description = ' + _mod + '.description'
	    demos.append((_description, _mod))
	except:
	    pass
demos.sort()

