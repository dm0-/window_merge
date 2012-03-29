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

AC_INIT([Window Merge],[0.2],
    [fedora.dm0@gmail.com],[window_merge],
    [https://github.com/dm0-/window_merge])
AM_INIT_AUTOMAKE
LT_INIT([disable-static win32-dll])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_DEFINE([PACKAGE_AUTHOR_UID],["dm0"],
    [Define to the short identifier of the author.])
AC_DEFINE([PACKAGE_AUTHOR_STRING],["David Michael <fedora.dm0@gmail.com>"],
    [Define to the full name and e-mail of the author.])

AS_CASE(["$host_os"],
    [*mingw*],AS_VAR_SET([LT_NO_UNDEFINED],[-no-undefined]),
    AS_VAR_SET([LT_NO_UNDEFINED]))
AC_SUBST([LT_NO_UNDEFINED])

PKG_CHECK_MODULES([pidgin],[pidgin],,
    [AC_MSG_WARN([[relying on environment pidgin_CFLAGS and pidgin_LIBS]])])
AC_SUBST([plugindir],['${libdir}/pidgin'])

AC_OUTPUT
