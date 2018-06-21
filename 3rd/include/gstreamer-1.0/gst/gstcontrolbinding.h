/* GStreamer
 *
 * Copyright (C) 2011 Stefan Sauer <ensonic@users.sf.net>
 *
 * gstcontrolbinding.h: Attachment for control sources
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

#ifndef __GST_CONTROL_BINDING_H__
#define __GST_CONTROL_BINDING_H__

#include <gst/gstconfig.h>

#include <glib-object.h>

#include <gst/gstcontrolsource.h>

G_BEGIN_DECLS

#define GST_TYPE_CONTROL_BINDING \
  (gst_control_binding_get_type())
#define GST_CONTROL_BINDING(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CONTROL_BINDING,GstControlBinding))
#define GST_CONTROL_BINDING_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CONTROL_BINDING,GstControlBindingClass))
#define GST_IS_CONTROL_BINDING(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CONTROL_BINDING))
#define GST_IS_CONTROL_BINDING_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CONTROL_BINDING))
#define GST_CONTROL_BINDING_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_CONTOL_SOURCE, GstControlBindingClass))

typedef struct _GstControlBinding GstControlBinding;
typedef struct _GstControlBindingClass GstControlBindingClass;

/**
 * GstControlBindingConvert:
 * @binding: the #GstControlBinding instance
 * @src_value: the value returned by the cotnrol source
 * @dest_value: the target GValue
 *
 * Function to map a control-value to the target GValue.
 */
typedef void (* GstControlBindingConvert) (GstControlBinding *binding, gdouble src_value, GValue *dest_value);

/**
 * GstControlBinding:
 * @name: name of the property of this binding
 * @pspec: #GParamSpec for this property
 *
 * The instance structure of #GstControlBinding.
 */
struct _GstControlBinding {
  GstObject parent;
  
  /*< public >*/
  gchar *name;
  GParamSpec *pspec;

  /*< private >*/
  GstObject *object;            /* GstObject owning the property
                                 * (== parent when bound) */
  gboolean disabled;

  gpointer _gst_reserved[GST_PADDING];
};

/**
 * GstControlBindingClass:
 * @parent_class: Parent class
 *
 * The class structure of #GstControlBinding.
 */

struct _GstControlBindingClass
{
  GstObjectClass parent_class;

  /* virtual methods */
  gboolean (* sync_values) (GstControlBinding *binding, GstObject *object, GstClockTime timestamp, GstClockTime last_sync);
  GValue * (* get_value) (GstControlBinding *binding, GstClockTime timestamp);
  gboolean (* get_value_array) (GstControlBinding *binding, GstClockTime timestamp,GstClockTime interval, guint n_values, gpointer values);
  gboolean (* get_g_value_array) (GstControlBinding *binding, GstClockTime timestamp,GstClockTime interval, guint n_values, GValue *values);

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING];
};

#define GST_CONTROL_BINDING_PSPEC(cb) (((GstControlBinding *) cb)->pspec)

GType gst_control_binding_get_type (void);

/* Functions */

gboolean            gst_control_binding_sync_values        (GstControlBinding * binding, GstObject *object,
                                                            GstClockTime timestamp, GstClockTime last_sync);
GValue *            gst_control_binding_get_value          (GstControlBinding *binding,
                                                            GstClockTime timestamp);
gboolean            gst_control_binding_get_value_array    (GstControlBinding *binding, GstClockTime timestamp,
                                                            GstClockTime interval, guint n_values, gpointer values);
gboolean            gst_control_binding_get_g_value_array  (GstControlBinding *binding, GstClockTime timestamp,
                                                            GstClockTime interval, guint n_values, GValue *values);

void                gst_control_binding_set_disabled       (GstControlBinding * binding, gboolean disabled);
gboolean            gst_control_binding_is_disabled        (GstControlBinding * binding);
G_END_DECLS

#endif /* __GST_CONTROL_BINDING_H__ */
