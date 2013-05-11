/**
 * @file utils.c
 * Provides a few miscellaneous routines that make life easier
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


/**
 * Given a parented widget, replace it and reparent it into a new container
 *
 * @param[in] child      The widget to be replaced and reparented
 * @param[in] swap       The widget that will be the replacement
 * @param[in] new_parent The container to adopt the displaced child widget
 *
 * @note This only supports existing parent widgets of type GtkPaned or GtkBox.
**/
void
pwm_widget_replace(GtkWidget *child, GtkWidget *swap, GtkWidget *new_parent)
{
  GtkWidget *parent;            /*< The parent of the widget to be replaced  */
  GValue val1 = G_VALUE_INIT;   /*< GtkContainer child property (manual get) */
  GValue val2 = G_VALUE_INIT;   /*< GtkContainer child property (manual get) */
  GtkPackType pack_type;        /*< GtkBox child property "pack-type"        */
  guint padding;                /*< GtkBox child property "padding"          */
  gboolean expand;              /*< GtkBox child property "expand"           */
  gboolean fill;                /*< GtkBox child property "fill"             */
  gboolean is_pane1 = FALSE;    /*< Whether the child is the first pane      */
  gboolean should_unparent;     /*< Whether the replacement is parented      */

  /* Sanity check: If not given a child or replacement, we've nothing to do. */
  if ( child == NULL || swap == NULL )
    return;

  parent = gtk_widget_get_parent(child);
  should_unparent = GTK_IS_CONTAINER(gtk_widget_get_parent(swap));

  /* For a GtkPaned parent, retrieve the current child widget properties. */
  if ( GTK_IS_PANED(parent) ) {
    g_value_init(&val1, G_TYPE_BOOLEAN);
    gtk_container_child_get_property(GTK_CONTAINER(parent), child,
                                     "resize", &val1);
    g_value_init(&val2, G_TYPE_BOOLEAN);
    gtk_container_child_get_property(GTK_CONTAINER(parent), child,
                                     "shrink", &val2);
    is_pane1 = gtk_paned_get_child1(GTK_PANED(parent)) == child;
  }

  /* For a GtkBox parent, retrieve the current child widget properties. */
  else if ( GTK_IS_BOX(parent) ) {
    g_value_init(&val1, G_TYPE_INT);
    gtk_container_child_get_property(GTK_CONTAINER(parent), child,
                                     "position", &val1);
    gtk_box_query_child_packing(GTK_BOX(parent), child,
                                &expand, &fill, &padding, &pack_type);
  }

  /* Unparent if the replacement already has a parent container. */
  if ( should_unparent ) {
    g_object_ref_sink(G_OBJECT(swap));
    gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(swap)), swap);
  }

  /* If no one is willing to adopt the orphaned child, it must be destroyed. */
  if ( new_parent != NULL )
    gtk_widget_reparent(child, new_parent);
  else
    gtk_widget_destroy(child);

  /* For a GtkPaned parent, add the new widget and restore child properties. */
  if ( GTK_IS_PANED(parent) ) {
    if ( is_pane1 )
      gtk_paned_pack1(GTK_PANED(parent), swap,
                      g_value_get_boolean(&val1), g_value_get_boolean(&val2));
    else
      gtk_paned_pack2(GTK_PANED(parent), swap,
                      g_value_get_boolean(&val1), g_value_get_boolean(&val2));
  }

  /* For a GtkBox parent, add the new widget and restore child properties. */
  else if ( GTK_IS_BOX(parent) ) {
    if ( pack_type == GTK_PACK_START )
      gtk_box_pack_start(GTK_BOX(parent), swap, expand, fill, padding);
    else
      gtk_box_pack_end(GTK_BOX(parent), swap, expand, fill, padding);
    gtk_box_reorder_child(GTK_BOX(parent), swap, g_value_get_int(&val1));
  }

  /* Remove the replacement's temporary reference that avoided destruction. */
  if ( should_unparent )
    g_object_unref(G_OBJECT(swap));
}
