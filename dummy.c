/**
 * @file dummy.c
 * Separates the details of the dummy conversation tab from the fray
 *
 * The dummy tab is a monstrosity bearing the original intention of simplifying
 * an area to which conversations could be dragged and attached.  It is only
 * supposed to be displayed when no conversations are open in a merged window.
 * Pidgin callbacks add and remove it as conversations come and go.
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

#include "plugin.h"

#include <gtkblist.h>
#include <gtkconv.h>
#include <gtkutils.h>

#include "window_merge.h"


/**
 * Allocate and return a conversation UI that only holds an instructions label
 *
 * @return               Pointer to the allocated conversation UI structure
 *
 * @note Remember pwm_free_dummy_conversation() for the window that owns this.
**/
void
pwm_init_dummy_conversation(PidginBuddyList *gtkblist)
{
  PidginConversation *gtkconv;  /*< The new (pretend) conversation structure */
  gchar *html;                  /*< The HTML-formatted instructions text     */
  gchar *pretty;                /*< The HTML text with prettier arrow chars  */

  gtkconv = g_new0(PidginConversation, 1);

  /* Define the label widget to be accepted as a conversation tab. */
  gtkconv->tab_cont = gtk_label_new(NULL);
  gtk_label_set_line_wrap(GTK_LABEL(gtkconv->tab_cont), TRUE);
  gtk_misc_set_alignment(GTK_MISC(gtkconv->tab_cont), 0.5f, 0.2f);
  g_object_set_data(G_OBJECT(gtkconv->tab_cont),
                    "PidginConversation", gtkconv);

  /* TRANSLATORS: A few notes on this one:
     1) Try to keep the "->" styled arrows to denote menu selection, since
        Pidgin converts those character sequences to Unicode arrows.
     2) The first two inserted strings are both the name of the plugin, and the
        third is the name of the conversation placement option.
     3) The HTML formatting can be adjusted if it makes sense in a particular
        locale, but try to keep it consistent with the instructions tab
        displayed in the Buddy List before the user defines any accounts. */
  html = g_strdup_printf(_(""
           "<span size='larger' weight='bold'>%s is enabled!</span>\n\n"
           "This space is reserved by the %s plugin to display "
           "conversations.  Individual conversations can be dragged to this "
           "area to attach them, or the conversation placement preference at "
           "<b>Tools->Preferences</b> can be set to <i>%s</i> to open all new "
           "conversations here by default."),
          _(PWM_STR_NAME), _(PWM_STR_NAME), _(PWM_STR_CP_BLIST));
  pretty = pidgin_make_pretty_arrows(html);
  g_free(html);
  gtk_label_set_markup(GTK_LABEL(gtkconv->tab_cont), pretty);
  g_free(pretty);

  /* Set up the label so it accepts dropped conversations like the infopane. */
  gtkconv->entry = gtkconv->tab_cont;
  gtkconv->infopane = gtkconv->tab_cont;
  gtkconv->infopane_hbox = gtkconv->tab_cont;

  /* Store the dummy conversation's pointer on the Buddy List. */
  pwm_store(gtkblist, "pwm_fake_tab", gtkconv);
}


/**
 * Display an instructions tab in the given window
 *
 * @param[in] gtkblist   The Buddy List to display an instructions tab
**/
void
pwm_show_dummy_conversation(PidginBuddyList *gtkblist)
{
  PidginConversation *gtkconv;  /*< The fake conversation structure          */
  PidginWindow *gtkconvwin;     /*< The conversation window tied to gtkblist */

  gtkconv = pwm_fetch(gtkblist, "pwm_fake_tab");
  gtkconvwin = pwm_blist_get_convs(gtkblist);

  /* Sanity check: Ensure the Buddy List is merged and not showing the tab. */
  if ( gtkconvwin == NULL || pidgin_conv_get_window(gtkconv) != NULL )
    return;

  /* Add the instructions tab to the conversations notebook. */
  pidgin_conv_window_add_gtkconv(gtkconvwin, gtkconv);

  /* Remove the "close" button that was just added to the tab label. */
  gtk_widget_destroy(gtkconv->close);
  gtkconv->close = NULL;

  /* Show the plugin name and an About icon for those who can see the label. */
  gtk_label_set_text(GTK_LABEL(gtkconv->tab_label), _(PWM_STR_NAME));
  gtk_label_set_text(GTK_LABEL(gtkconv->menu_label), _(PWM_STR_NAME));
  g_object_set(G_OBJECT(gtkconv->icon), "stock", GTK_STOCK_ABOUT, NULL);
  g_object_set(G_OBJECT(gtkconv->menu_icon), "stock", GTK_STOCK_ABOUT, NULL);
}


/**
 * Take down the instructions tab from its parent window
 *
 * @param[in] gtkblist   The Buddy List that owns the dummy conversation UI
**/
void
pwm_hide_dummy_conversation(PidginBuddyList *gtkblist)
{
  PidginConversation *gtkconv;  /*< The fake conversation structure          */
  PidginWindow *gtkconvwin;     /*< The conversation window that has gtkconv */

  gtkconv = pwm_fetch(gtkblist, "pwm_fake_tab");
  gtkconvwin = pidgin_conv_get_window(gtkconv);

  /* Sanity check: If the dummy tab isn't being shown, leave it alone. */
  if ( gtkconvwin == NULL )
    return;

  /* Force-unparent the dummy tab before the slew of callbacks run over it. */
  /* XXX: This is bad, but it stops Message Notifications from exploding. */
  gtkconvwin->gtkconvs = g_list_remove(gtkconvwin->gtkconvs, gtkconv);

  /* Remove the tab from its window. */
  gtkconv->win = NULL;
  pidgin_conv_window_remove_gtkconv(gtkconvwin, gtkconv);
}


/**
 * Free the memory used by the instructions tab
 *
 * @param[in] gtkblist   The Buddy List that owns the dummy conversation UI
**/
void
pwm_free_dummy_conversation(PidginBuddyList *gtkblist)
{
  PidginConversation *gtkconv;  /*< The fake conversation structure          */

  gtkconv = pwm_fetch(gtkblist, "pwm_fake_tab");

  /* Sanity check: Ensure the Buddy List has an associated dummy tab. */
  if ( gtkconv == NULL )
    return;

  /* Destroy the label widget, and release the conversation UI memory. */
  gtk_widget_destroy(gtkconv->tab_cont);
  pwm_free(gtkblist, "pwm_fake_tab");
}
