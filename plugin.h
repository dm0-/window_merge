/**
 * @file plugin.h
 * Prepares definitions to abstract project information passed from configure
 *
 * It also sets up definitions required by other headers for translations and
 * building Pidgin plugins properly.  This header should therefore be included
 * in source files before any system headers.
 *
 * @section LICENSE
 * Copyright (C) 2012 David Michael <fedora.dm0@gmail.com>
 *
 * This file is part of Window Merge.
 *
 * Window Merge is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Window Merge is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Window Merge.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#ifdef __G_LIB_H__
#error "The file plugin.h must be included first to properly build the plugin."
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define PLUGIN_AUTHOR  "dm0"
#define PLUGIN_ID      PLUGIN_TYPE "-" PLUGIN_AUTHOR "-" PLUGIN_TOKEN
#define PLUGIN_NAME    PWM_STR_NAME
#define PLUGIN_TOKEN   PACKAGE_TARNAME
#define PLUGIN_TYPE    "gtk"
#define PLUGIN_URL     PACKAGE_URL
#define PLUGIN_VERSION PACKAGE_VERSION

#define PREF_ROOT   "/plugins/" PLUGIN_TYPE "/" PLUGIN_TOKEN
#define PREF_HEIGHT PREF_ROOT "/blist_height"
#define PREF_WIDTH  PREF_ROOT "/blist_width"
#define PREF_SIDE   PREF_ROOT "/convs_side"

/* Tell the libpurple headers to build this correctly. */
#define PURPLE_PLUGINS

/* Set up a convenient flag for strict compiling with unused callback parms. */
#ifdef __GNUC__
#define U __attribute__((unused))
#else
#define U /* unused */
#endif

/* Check Pidgin's localedir if a path was not explicitly specified. */
#ifndef LOCALEDIR
#define LOCALEDIR bindtextdomain("pidgin", NULL)
#endif

/* Define the translation domain to appear as a subpackage of Pidgin. */
#define GETTEXT_PACKAGE "pidgin-" PLUGIN_TOKEN

/* Determine whether to enable translations based on configure options. */
#ifdef ENABLE_NLS
#include <glib/gi18n-lib.h>
#else
#define  _(s) (s)
#define N_(s) (s)
#endif

#define PWM_STR_NAME N_("Window Merge")

#define PWM_STR_CP_BLIST N_("Buddy List window")

#endif /* ifndef __PLUGIN_H__ */
