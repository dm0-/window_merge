/**
 * @file utils.c
 * Provides routines for associating arbitrary data with merged windows
 *
 * The real purpose of the functions in this file is to take what would be
 * global variables and glue them to an instance of a Buddy List.  This will
 * allow easier support for independent settings across multiple Buddy Lists.
 *
 * Note that data is associated with the Buddy List's window widget, which is
 * also assigned in the conversation's structure.  This allows the data to be
 * accessed from either structure (if that was really necessary).
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

#include <gtkblist.h>
#include <gtkconv.h>


/**
 * Return the conversation window structure merged into the given Buddy List
 *
 * @param[in] gtkblist   The Buddy List whose conversation window is requested
 * @return               The conversation window structure tied to gtkblist
**/
PidginWindow *
pwm_blist_get_convs(PidginBuddyList *gtkblist)
{
  if ( gtkblist == NULL )
    return NULL;

  return g_object_get_data(G_OBJECT(gtkblist->notebook), "pwm_convs");
}


/**
 * Return the Buddy List structure merged into the given conversation window
 *
 * @param[in] gtkconvwin The conversation window whose Buddy List is requested
 * @return               The Buddy List structure tied to gtkconvwin
**/
PidginBuddyList *
pwm_convs_get_blist(PidginWindow *gtkconvwin)
{
  if ( gtkconvwin == NULL )
    return NULL;

  return g_object_get_data(G_OBJECT(gtkconvwin->notebook), "pwm_blist");
}
