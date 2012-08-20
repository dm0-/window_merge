/**
 * @file merge.c
 * Deals with the widgets of the merged window panes and menu bar
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
#include <gtkimhtml.h>

#include <prefs.h>

#include <gdk/gdkkeysyms.h>

#include "window_merge.h"


/**
 * A callback for when the position of a GtkPaned slider changes
 *
 * This function is responsible for storing the width or height of the Buddy
 * List as a preference after the user changes it by dragging the slider.
 *
 * @param[in] gobject    Pointer to the GtkPaned structure that was resized
 * @param[in] pspec      Unused
 * @param[in] data       Pointer to the Buddy List that is a parent of gobject
**/
static void
notify_position_cb(GObject *gobject, U GParamSpec *pspec, gpointer data)
{
  PidginBuddyList *gtkblist;    /*< Buddy List window containing these panes */
  gint max_position;            /*< The "max-position" property of gobject   */
  gint size;                    /*< Current size of the Buddy List pane      */

  gtkblist = data;
  size = gtk_paned_get_position(GTK_PANED(gobject));

  /* If the Buddy List is not the first pane, invert the size preference. */
  if ( gtk_paned_get_child1(GTK_PANED(gobject)) != gtkblist->notebook ) {
    g_object_get(gobject, "max-position", &max_position, NULL);
    size = max_position - size;
  }

  /* Store this size as a user preference (depending on paned orientation). */
  if ( GTK_IS_VPANED(gobject) )
    purple_prefs_set_int(PREF_HEIGHT, size);
  else
    purple_prefs_set_int(PREF_WIDTH, size);
}


/**
 * A callback for when the total size of a GtkPaned changes
 *
 * This should be called after a new GtkPaned finds its parent and calculates
 * its "max-position" property.  It is only intended to be run on this single
 * occassion, so it removes itself on completion.  The call is used to set the
 * initial size of the Buddy List to the user's preference.
 *
 * @param[in] gobject    Pointer to the GtkPaned structure that was resized
 * @param[in] pspec      Unused
 * @param[in] data       Pointer to the Buddy List that is a parent of gobject
**/
static void
notify_max_position_cb(GObject *gobject, U GParamSpec *pspec, gpointer data)
{
  PidginBuddyList *gtkblist;    /*< Buddy List window containing these panes */
  gint max_position;            /*< The "max-position" property of gobject   */
  gint size;                    /*< Desired size of the Buddy List pane      */

  gtkblist = data;

  /* Fetch the user's preferred Buddy List size (depending on orientation). */
  if ( GTK_IS_VPANED(gobject) )
    size = purple_prefs_get_int(PREF_HEIGHT);
  else
    size = purple_prefs_get_int(PREF_WIDTH);

  /* If the Buddy List is not the first pane, invert the size preference. */
  if ( gtk_paned_get_child1(GTK_PANED(gobject)) != gtkblist->notebook ) {
    g_object_get(gobject, "max-position", &max_position, NULL);
    size = max_position - size;
  }

  /* Adjust the panes' slider to set the Buddy List to its desired size. */
  gtk_paned_set_position(GTK_PANED(gobject), size);

  /* Disconnect this callback.  This initial setting was only needed once. */
  g_object_disconnect(gobject, "any_signal",
                      G_CALLBACK(notify_max_position_cb), data, NULL);

  /* Now that system-induced slider changes are done, monitor user changes. */
  g_object_connect(gobject, "signal::notify::position",
                   G_CALLBACK(notify_position_cb), data, NULL);
}


/**
 * Pass a focus (in) event from one widget to another
 *
 * This is only used to trigger the signal handlers on the conversation window
 * for removing message notifications when the Buddy List window is focused.
 *
 * @param[in] widget     Unused
 * @param[in] event      Unused
 * @param[in] data       Pointer to the widget to receive the passed event
 * @return               Whether to stop processing other event handlers
**/
static gboolean
focus_in_event_cb(U GtkWidget *widget, U GdkEventFocus *event, gpointer data)
{
  GtkWidget *other_widget;      /*< Widget pretending to receive an event    */
  GdkWindow *window;            /*< Window of the widget receiving the event */
  GdkEvent *focus;              /*< New focus event for the specified widget */

  focus = gdk_event_new(GDK_FOCUS_CHANGE);
  other_widget = data;
  window = gtk_widget_get_window(other_widget);

  focus->focus_change.window = g_object_ref(window);
  focus->focus_change.send_event = TRUE;
  focus->focus_change.in = TRUE;

  gtk_widget_event(other_widget, focus);

  gdk_event_free(focus);

  return FALSE;
}


/**
 * Create a conversation window and merge it with the given Buddy List window
 *
 * This is the real core of the plugin right here.  It initializes the Buddy
 * List with a conversation window just like the project advertises.  See the
 * function pwm_split_conversation() to reverse this effect.
 *
 * @param[in] gtkblist   The Buddy List that will be able to show conversations
**/
void
pwm_merge_conversation(PidginBuddyList *gtkblist)
{
  PidginWindow *gtkconvwin;     /*< The mutilated conversations for gtkblist */
  GtkBindingSet *binding_set;   /*< The binding set of GtkIMHtml widgets     */

  /* Sanity check: If the Buddy List is already merged, don't mess with it. */
  if ( pwm_blist_get_convs(gtkblist) != NULL )
    return;

  binding_set = gtk_binding_set_by_class(g_type_class_ref(GTK_TYPE_IMHTML));
  gtkconvwin = pidgin_conv_window_new();

  /* Tie the Buddy List and conversation window instances together. */
  g_object_set_data(G_OBJECT(gtkblist->notebook), "pwm_convs", gtkconvwin);
  g_object_set_data(G_OBJECT(gtkconvwin->notebook), "pwm_blist", gtkblist);

  /* Backup the Buddy List window title for restoring it later. */
  pwm_store(gtkblist, "title",
            g_strdup(gtk_window_get_title(GTK_WINDOW(gtkblist->window))));

  /* Move the conversation notebook into the Buddy List window. */
  pwm_create_paned_layout(gtkblist, purple_prefs_get_string(PREF_SIDE));

  /* Display the instructions tab for new users. */
  pwm_init_dummy_conversation(gtkblist);
  pwm_show_dummy_conversation(gtkblist);

  /* Pass focus events from Buddy List to conversation window. */
  g_object_connect(G_OBJECT(gtkblist->window), "signal::focus-in-event",
                   G_CALLBACK(focus_in_event_cb), gtkconvwin->window, NULL);

  /* Point the conversation window structure at the Buddy List's window. */
  pwm_store(gtkblist, "conv_window", gtkconvwin->window);
  gtkconvwin->window = gtkblist->window;

  /* Block these "move-cursor" bindings for conversation event handlers. */
  /* XXX: These are skipped in any GtkIMHtml, not just the conversations. */
  /* XXX: Furthermore, there is no event to undo this effect. */
  gtk_binding_entry_skip(binding_set, GDK_Up,           GDK_CONTROL_MASK);
  gtk_binding_entry_skip(binding_set, GDK_Down,         GDK_CONTROL_MASK);
  gtk_binding_entry_skip(binding_set, GDK_Page_Up,      GDK_CONTROL_MASK);
  gtk_binding_entry_skip(binding_set, GDK_Page_Down,    GDK_CONTROL_MASK);
  gtk_binding_entry_skip(binding_set, GDK_KP_Page_Up,   GDK_CONTROL_MASK);
  gtk_binding_entry_skip(binding_set, GDK_KP_Page_Down, GDK_CONTROL_MASK);
}


/**
 * Restore the Buddy List to its former glory by splitting off conversations
 *
 * This effectively will undo everything done by pwm_merge_conversation().  The
 * Buddy List should be returned to its original state, and any conversations
 * should be in a separate window.
 *
 * @param[in] gtkblist   The Buddy List that has had enough of this plugin
**/
void
pwm_split_conversation(PidginBuddyList *gtkblist)
{
  PidginWindow *gtkconvwin;     /*< Conversation window merged into gtkblist */
  GtkWidget *paned;             /*< The panes on the Buddy List window       */
  gchar *title;                 /*< Original title of the Buddy List window  */

  gtkconvwin = pwm_blist_get_convs(gtkblist);
  paned = pwm_fetch(gtkblist, "paned");
  title = pwm_fetch(gtkblist, "title");

  /* Ensure the conversation window's menu items are returned. */
  pwm_set_conv_menus_visible(gtkblist, FALSE);

  /* End the association between the Buddy List and its conversation window. */
  g_object_steal_data(G_OBJECT(gtkblist->notebook), "pwm_convs");
  g_object_steal_data(G_OBJECT(gtkconvwin->notebook), "pwm_blist");

  /* Point the conversation window's structure back to its original window. */
  gtkconvwin->window = pwm_fetch(gtkblist, "conv_window");
  pwm_clear(gtkblist, "conv_window");

  /* Stop passing focus events from Buddy List to conversation window. */
  g_object_disconnect(G_OBJECT(gtkblist->window), "any_signal",
                      G_CALLBACK(focus_in_event_cb), gtkconvwin->window, NULL);

  /* Restore the conversation window's notebook. */
  pwm_widget_replace(pwm_fetch(gtkblist, "placeholder"),
                     gtkconvwin->notebook, NULL);
  pwm_clear(gtkblist, "placeholder");

  /* Free the dummy conversation, and display the window if it survives. */
  pwm_free_dummy_conversation(gtkblist);
  if ( g_list_find(pidgin_conv_windows_get_list(), gtkconvwin) != NULL )
    pidgin_conv_window_show(gtkconvwin);

  /* Restore the Buddy List's original structure, and destroy the panes. */
  pwm_widget_replace(paned, gtkblist->notebook, NULL);
  pwm_clear(gtkblist, "paned");

  /* Restore the window title and icons from before conversations set them. */
  gtk_window_set_icon_list(GTK_WINDOW(gtkblist->window), NULL);
  gtk_window_set_title(GTK_WINDOW(gtkblist->window), title);
  g_free(title);
  pwm_clear(gtkblist, "title");
}


/**
 * Construct (or reconstruct when settings change) the window's paned layout
 *
 * It is worth noting that the string preferences only use the first character
 * to determine orientation since they are all unique (and it avoids calling
 * extra string functions).  The full strings are just for readable prefs.xml.
 *
 * @param[in] gtkblist   The Buddy List that needs a new paned window structure
 * @param[in] side       The pref where convs are placed relative to the blist
 *
 * @note This is structured to default to "right" on an invalid pref setting.
**/
void
pwm_create_paned_layout(PidginBuddyList *gtkblist, const char *side)
{
  PidginWindow *gtkconvwin;     /*< Conversation window merged into gtkblist */
  GtkWidget *old_paned;         /*< The existing paned layout, if it exists  */
  GtkWidget *paned;             /*< The new layout panes being created       */
  GtkWidget *placeholder;       /*< Marks the conv notebook's original spot  */
  GValue value = G_VALUE_INIT;  /*< For passing a property value to a widget */

  gtkconvwin = pwm_blist_get_convs(gtkblist);
  old_paned = pwm_fetch(gtkblist, "paned");

  /* Create the requested vertical or horizontal paned layout. */
  if ( side != NULL && (*side == 't' || *side == 'b') )
    paned = gtk_vpaned_new();
  else
    paned = gtk_hpaned_new();
  gtk_widget_show(paned);
  pwm_store(gtkblist, "paned", paned);

  /* When the size of the panes is determined, reset the Buddy List size. */
  g_object_connect(G_OBJECT(paned), "signal::notify::max-position",
                   G_CALLBACK(notify_max_position_cb), gtkblist, NULL);

  /* If the Buddy List is pristine, make the panes and replace its notebook. */
  if ( old_paned == NULL ) {
    placeholder = gtk_label_new(NULL);
    if ( side != NULL && (*side == 't' || *side == 'l') ) {
      pwm_widget_replace(gtkconvwin->notebook, placeholder, paned);
      pwm_widget_replace(gtkblist->notebook, paned, paned);
    } else {
      pwm_widget_replace(gtkblist->notebook, paned, paned);
      pwm_widget_replace(gtkconvwin->notebook, placeholder, paned);
    }
    pwm_store(gtkblist, "placeholder", placeholder);
  }

  /* If existing panes are being replaced, define the new layout and use it. */
  else {
    if ( side != NULL && (*side == 't' || *side == 'l') ) {
      gtk_widget_reparent(gtkconvwin->notebook, paned);
      gtk_widget_reparent(gtkblist->notebook, paned);
    } else {
      gtk_widget_reparent(gtkblist->notebook, paned);
      gtk_widget_reparent(gtkconvwin->notebook, paned);
    }
    pwm_widget_replace(old_paned, paned, NULL);
  }

  /* Make conversations resize with the window so the Buddy List is fixed. */
  g_value_init(&value, G_TYPE_BOOLEAN);
  g_value_set_boolean(&value, TRUE);
  gtk_container_child_set_property(GTK_CONTAINER(paned), gtkconvwin->notebook,
                                   "resize", &value);
  g_value_set_boolean(&value, FALSE);
  gtk_container_child_set_property(GTK_CONTAINER(paned), gtkblist->notebook,
                                   "resize", &value);
}


/**
 * Toggle the visibility of conversation window menu items
 *
 * The method of hiding the menu items is to reparent them back into their
 * original (still hidden) parent window.  This avoids conflicting with other
 * "hiding" methods used by plugins, such as widget sensitivity or visibility.
 *
 * When migrating to the Buddy List, left-justified items are inserted after
 * the last left-justified item.  Right-justified items are inserted before the
 * first right-justified item.  This appears to append conversation menu items
 * while keeping the Buddy List's notification icon on the far right.
 *
 * When migrating back to the conversation window, left-justified items are
 * prepended to all items.  Right-justified items are inserted before the first
 * right-justified item.  This gives the appearance of appending any newly
 * added menu items when they are all migrated to the Buddy List again.
 *
 * @param[in] gtkblist   The Buddy List whose menu needs adjusting
 * @param[in] visible    Whether the menu items are being shown or hidden
**/
void
pwm_set_conv_menus_visible(PidginBuddyList *gtkblist, gboolean visible)
{
  PidginWindow *gtkconvwin;     /*< Conversation window merged into gtkblist */
  GtkMenu *submenu;             /*< A submenu of a conversation menu item    */
  GtkContainer *from_menu;      /*< Menu bar of the window losing the items  */
  GtkContainer *to_menu;        /*< Menu bar of the window gaining the items */
  GtkWidget *blist_menu;        /*< The Buddy List menu bar                  */
  GtkWidget *convs_menu;        /*< The conversation window menu bar         */
  GtkWidget *item;              /*< A menu item widget being transferred     */
  GList *children;              /*< List of menu items in a given window     */
  GList *child;                 /*< A menu item in the list (iteration)      */
  GList *migrated_items;        /*< List of items added to the Buddy List    */
  gint index_left;              /*< Position to insert left-justified items  */
  gint index_right;             /*< Position to insert right-justified items */

  gtkconvwin = pwm_blist_get_convs(gtkblist);

  /* Sanity check: Only act on a merged Buddy List window. */
  if ( gtkconvwin == NULL )
    return;

  blist_menu = gtk_widget_get_parent(gtkblist->menutray);
  convs_menu = gtkconvwin->menu.menubar;
  from_menu = GTK_CONTAINER(visible ? convs_menu : blist_menu);
  to_menu   = GTK_CONTAINER(visible ? blist_menu : convs_menu);
  migrated_items = pwm_fetch(gtkblist, "conv_menus");

  /* Locate the position before the first right-aligned menu item. */
  index_right = 0;
  children = gtk_container_get_children(to_menu);
  for ( child = children; child != NULL; child = child->next )
    if ( gtk_menu_item_get_right_justified(GTK_MENU_ITEM(child->data)) )
      break;
    else
      index_right++;
  g_list_free(children);
  index_left = visible ? index_right : 0;

  /* Loop over each conversation menu item to move it to its destination. */
  children = visible ? gtk_container_get_children(from_menu) : migrated_items;
  for ( child = children; child != NULL; child = child->next ) {
    item = GTK_WIDGET(child->data);

    /* Reparent the item into the window based on existing justified items. */
    g_object_ref_sink(G_OBJECT(item));
    gtk_container_remove(from_menu, item);
    if ( gtk_menu_item_get_right_justified(GTK_MENU_ITEM(item)) )
      gtk_menu_shell_insert(GTK_MENU_SHELL(to_menu), item, index_right);
    else
      gtk_menu_shell_insert(GTK_MENU_SHELL(to_menu), item, index_left++);
    g_object_unref(G_OBJECT(item));
    index_right++;

    /* Register/Unregister its accelerator group with the Buddy List window. */
    submenu = GTK_MENU(gtk_menu_item_get_submenu(GTK_MENU_ITEM(item)));
    if ( visible )
      gtk_window_add_accel_group(GTK_WINDOW(gtkblist->window),
                                 gtk_menu_get_accel_group(submenu));
    else
      gtk_window_remove_accel_group(GTK_WINDOW(gtkblist->window),
                                    gtk_menu_get_accel_group(submenu));

    /* Add this menu item to the list for later restoration, if necessary. */
    if ( visible )
      migrated_items = g_list_append(migrated_items, child->data);
  }
  g_list_free(children);

  /* Update the stored pointer to the list of migrated items. */
  if ( visible )
    pwm_store(gtkblist, "conv_menus", migrated_items);
  else
    pwm_clear(gtkblist, "conv_menus");
}
