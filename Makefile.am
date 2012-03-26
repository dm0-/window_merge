# Copyright (C) 2012 David Michael <fedora.dm0@gmail.com>
#
# This file is part of Window Merge.
#
# Window Merge is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# Window Merge is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# Window Merge.  If not, see <http://www.gnu.org/licenses/>.

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = window_merge.la
window_merge_la_CFLAGS  = $(pidgin_CFLAGS)
window_merge_la_LDFLAGS = -avoid-version -export-dynamic -module -shared \
                          $(LT_NO_UNDEFINED) \
                          $(pidgin_LIBS)
window_merge_la_SOURCES = dummy.c merge.c plugin.c utils.c
