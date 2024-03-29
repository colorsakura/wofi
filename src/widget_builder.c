/*
 *  Copyright (C) 2020-2022 Scoopta
 *  This file is part of Wofi
 *  Wofi is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wofi is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wofi.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <widget_builder.h>

#include <utils.h>
#include <wofi.h>

struct widget_builder *wofi_widget_builder_init(struct mode *mode,
                                                size_t actions) {
  struct widget_builder *builder =
      calloc(actions, sizeof(struct widget_builder));

  for (size_t count = 0; count < actions; ++count) {
    builder[count].mode = mode;
    builder[count].box =
        WOFI_PROPERTY_BOX(wofi_property_box_new(GTK_ORIENTATION_HORIZONTAL, 0));

    if (count == 0) {
      builder->actions = actions;
    }
  }
  return builder;
}

void wofi_widget_builder_set_search_text(struct widget_builder *builder,
                                         char *search_text) {
  wofi_property_box_add_property(builder->box, "filter", search_text);
}

void wofi_widget_builder_set_action(struct widget_builder *builder,
                                    char *action) {
  wofi_property_box_add_property(builder->box, "action", action);
}

static void va_to_list(struct wl_list *classes, va_list args) {
  char *arg;
  while ((arg = va_arg(args, char *)) != NULL) {
    struct css_class *class = malloc(sizeof(struct css_class));
    class->class = arg;
    wl_list_insert(classes, &class->link);
  }
}

void wofi_widget_builder_insert_text(struct widget_builder *builder,
                                     const char *text, ...) {
  struct wl_list classes;
  wl_list_init(&classes);

  va_list args;
  va_start(args, text);
  va_to_list(&classes, args);
  va_end(args);

  wofi_widget_builder_insert_text_with_list(builder, text, &classes);

  struct css_class *node, *tmp;
  wl_list_for_each_safe(node, tmp, &classes, link) { free(node); }
}

void wofi_widget_builder_insert_text_with_list(struct widget_builder *builder,
                                               const char *text,
                                               struct wl_list *classes) {
  GtkWidget *label = gtk_label_new(text);
  gtk_container_add(GTK_CONTAINER(builder->box), label);
  gtk_widget_set_name(label, "text");

  GtkStyleContext *ctx = gtk_widget_get_style_context(label);

  struct css_class *node;
  wl_list_for_each(node, classes, link) {
    char *tmp = utils_concat(3, builder->mode->name, "-", node->class);
    gtk_style_context_add_class(ctx, tmp);
    free(tmp);
  }
}

void wofi_widget_builder_insert_image(struct widget_builder *builder,
                                      GdkPixbuf *pixbuf, ...) {
  struct wl_list classes;
  wl_list_init(&classes);

  va_list args;
  va_start(args, pixbuf);
  va_to_list(&classes, args);
  va_end(args);

  wofi_widget_builder_insert_image_with_list(builder, pixbuf, &classes);

  struct css_class *node, *tmp;
  wl_list_for_each_safe(node, tmp, &classes, link) { free(node); }
}

void wofi_widget_builder_insert_image_with_list(struct widget_builder *builder,
                                                GdkPixbuf *pixbuf,
                                                struct wl_list *classes) {
  GtkWidget *img = gtk_image_new();
  cairo_surface_t *surface = gdk_cairo_surface_create_from_pixbuf(
      pixbuf, wofi_get_window_scale(), gtk_widget_get_window(img));
  gtk_image_set_from_surface(GTK_IMAGE(img), surface);
  cairo_surface_destroy(surface);
  gtk_container_add(GTK_CONTAINER(builder->box), img);
  gtk_widget_set_name(img, "img");

  GtkStyleContext *ctx = gtk_widget_get_style_context(img);

  struct css_class *node;
  wl_list_for_each(node, classes, link) {
    char *tmp = utils_concat(3, builder->mode->name, "-", node->class);
    gtk_style_context_add_class(ctx, tmp);
    free(tmp);
  }
}

struct widget_builder *
wofi_widget_builder_get_idx(struct widget_builder *builder, size_t idx) {
  return builder + idx;
}

struct widget *wofi_widget_builder_get_widget(struct widget_builder *builder) {
  if (builder->actions == 0) {
    fprintf(stderr, "%s: This is not the 0 index of the widget_builder array\n",
            builder->mode->name);
    return NULL;
  }

  if (builder->widget == NULL) {
    builder->widget = malloc(sizeof(struct widget));
    builder->widget->builder = builder;
    builder->widget->action_count = builder->actions;
  }

  for (size_t count = 0; count < builder->actions; ++count) {
  }

  return builder->widget;
}

void wofi_widget_builder_free(struct widget_builder *builder) {
  if (builder->widget != NULL) {
    free(builder->widget);
  }
  free(builder);
}
