/* -*- Mode: C; c-basic-offset: 4 -*- */
/* PyGTK - python bindings for GTK+
 * Copyright (C) 1997-2000 James Henstridge <james@daa.com.au>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _PYGTK_CONFIG_H_
#define _PYGTK_CONFIG_H_
@TOP@

/* define if Numeric Python support is to be used */
#undef HAVE_NUMPY

#undef HAVE_ARRAYOBJECT_H
#undef HAVE_EXTENSIONS_ARRAYOBJECT_H
#undef HAVE_NUMERIC_ARRAYOBJECT_H

#undef ENABLE_PYGTK_THREADING

@BOTTOM@
#endif /* !_PYGTK_CONFIG_H_ */
