/**
 * @file window_merge.h
 * Defines all the prototypes of functions shared across plugin source files
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

#ifndef __WINDOW_MERGE_H__
#define __WINDOW_MERGE_H__

/* Functions for Merged Windows */
void pwm_merge_conversation(PidginBuddyList *);
void pwm_destroy_conversation(PidginBuddyList *);
void pwm_create_paned_layout(PidginBuddyList *, const char *);
void pwm_set_conv_menus_visible(PidginBuddyList *, gboolean);

/* Dummy Conversation Functions */
void pwm_init_dummy_conversation(PidginBuddyList *);
void pwm_show_dummy_conversation(PidginBuddyList *);
void pwm_hide_dummy_conversation(PidginBuddyList *);
void pwm_free_dummy_conversation(PidginBuddyList *);

/* Utility Functions */
PidginWindow *pwm_blist_get_convs(PidginBuddyList *);
PidginBuddyList *pwm_convs_get_blist(PidginWindow *);
void pwm_store(PidginBuddyList *, const char *, gpointer);
gpointer pwm_fetch(PidginBuddyList *, const char *);
void pwm_clear(PidginBuddyList *, const char *);
void pwm_free(PidginBuddyList *, const char *);
void pwm_destroy(PidginBuddyList *, const char *);

/* TRANSLATORS: This is the user-visible name of the plugin.  The name was
   intended to give a brief sense of what the plugin does, so feel free to be
   liberal in adjusting it to make sense for a given locale. */
#define PWM_STR_NAME N_("Window Merge")

/* TRANSLATORS: This is the name of the conversation placement option that is
   added to the list to attach conversations to the Buddy List by default. */
#define PWM_STR_CP_BLIST N_("Buddy List window")

#endif /* ifndef __WINDOW_MERGE_H__ */
