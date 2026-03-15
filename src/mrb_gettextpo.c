/**
 * Copyright (C) 2026  gemmaro
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "mrb_gettextpo.h"
#include <gettext-po.h>
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/boxing_word.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/hash.h>
#include <mruby/object.h>
#include <mruby/presym.h>
#include <mruby/range.h>
#include <mruby/value.h>
#include <mruby/variable.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define DONE mrb_gc_arena_restore (mrb, 0);

#define MODULE mrb_module_get_id (mrb, MRB_SYM (GettextPO))
#define MESSAGE mrb_class_get_under_id (mrb, MODULE, MRB_SYM (Message))
#define FLAG_ITERATOR                                                         \
  mrb_class_get_under_id (mrb, MODULE, MRB_SYM (FlagIterator))
#define ERROR mrb_class_get_under_id (mrb, MODULE, MRB_SYM (Error))

#define NIL mrb_nil_value ()

static mrb_value
gettextpo_m_header_entry_value (mrb_state *mrb, mrb_value klass)
{
  char *header = NULL, *field = NULL;
  mrb_get_args (mrb, "zz", &header, &field);

  char *string = po_header_field (header, field);
  if (string)
    {
      mrb_value value = mrb_str_new_cstr (mrb, string);
      free (string);
      return value;
    }
  else
    return NIL;
}

static mrb_value
gettextpo_m_header_with_updated_entry_value (mrb_state *mrb, mrb_value klass)
{
  char *header = NULL, *field = NULL, *value = NULL;
  mrb_get_args (mrb, "zzz", &header, &field, &value);

  char *updated_header = po_header_set_field (header, field, value);
  mrb_value header_value = mrb_str_new_cstr (mrb, updated_header);
  free (updated_header);
  return header_value;
}

static void
gettextpo_flag_iterator_free (mrb_state *mrb, void *iter)
{
  if (iter)
    po_flag_iterator_free (iter);
}

static const struct mrb_data_type gettextpo_flag_iterator_data_type = {
  .struct_name = "gettextpo flag iterator",
  .dfree = gettextpo_flag_iterator_free,
};

static mrb_value
gettextpo_flag_iterator_m_next (mrb_state *mrb, mrb_value self)
{
  const char *flag = po_flag_next (DATA_PTR (self));
  return flag ? mrb_str_new_cstr (mrb, flag) : mrb_nil_value ();
}

static void
gettextpo_message_free (mrb_state *mrb, void *message)
{
  /* NOTE: memory is freed by libgettextpo file struct */
}

static const struct mrb_data_type gettextpo_message_data_type = {
  .struct_name = "gettextpo message",
  .dfree = gettextpo_message_free,
};

#define OPTIONAL_STRING_GETTER(field)                                         \
  static mrb_value gettextpo_message_m_##field (mrb_state *mrb,               \
                                                mrb_value self)               \
  {                                                                           \
    const char *string = po_message_##field (DATA_PTR (self));                \
    return string ? mrb_str_new_cstr (mrb, string) : NIL;                     \
  }
#define OPTIONAL_STRING_SETTER(field)                                         \
  static mrb_value gettextpo_message_m_##field##_set (mrb_state *mrb,         \
                                                      mrb_value self)         \
  {                                                                           \
    char *string = NULL;                                                      \
    mrb_get_args (mrb, "z!", &string);                                        \
    po_message_set_##field (DATA_PTR (self), string);                         \
    return NIL;                                                               \
  }

OPTIONAL_STRING_GETTER (msgctxt);
OPTIONAL_STRING_SETTER (msgctxt);

#define STRING_GETTER(field)                                                  \
  static mrb_value gettextpo_message_m_##field (mrb_state *mrb,               \
                                                mrb_value self)               \
  {                                                                           \
    return mrb_str_new_cstr (mrb, po_message_##field (DATA_PTR (self)));      \
  }
#define STRING_SETTER(field)                                                  \
  static mrb_value gettextpo_message_m_##field##_set (mrb_state *mrb,         \
                                                      mrb_value self)         \
  {                                                                           \
    char *string = NULL;                                                      \
    mrb_get_args (mrb, "z", &string);                                         \
    po_message_set_##field (DATA_PTR (self), string);                         \
    return NIL;                                                               \
  }

STRING_GETTER (msgid);
STRING_SETTER (msgid);
STRING_GETTER (msgid_plural);
STRING_SETTER (msgid_plural);
STRING_GETTER (msgstr);
STRING_SETTER (msgstr);

static mrb_value
gettextpo_message_m_msgstr_plural (mrb_state *mrb, mrb_value self)
{
  mrb_int index = 0;
  mrb_get_args (mrb, "i", &index);

  const char *msgstr_plural
      = po_message_msgstr_plural (DATA_PTR (self), index);
  return msgstr_plural ? mrb_str_new_cstr (mrb, msgstr_plural) : NIL;
}

static mrb_value
gettextpo_message_m_msgstr_plural_set (mrb_state *mrb, mrb_value self)
{
  int index = 0;
  char *msgstr_plural = NULL;
  mrb_get_args (mrb, "iz!", &index, &msgstr_plural);

  po_message_set_msgstr_plural (DATA_PTR (self), index, msgstr_plural);
  return NIL;
}

STRING_GETTER (comments);
STRING_SETTER (comments);
STRING_GETTER (extracted_comments);
STRING_SETTER (extracted_comments);
OPTIONAL_STRING_GETTER (prev_msgctxt);
OPTIONAL_STRING_SETTER (prev_msgctxt);
OPTIONAL_STRING_GETTER (prev_msgid);
OPTIONAL_STRING_SETTER (prev_msgid);
OPTIONAL_STRING_GETTER (prev_msgid_plural);
OPTIONAL_STRING_SETTER (prev_msgid_plural);

#define BOOL_GETTER(field)                                                    \
  static mrb_value gettextpo_message_m_##field (mrb_state *mrb,               \
                                                mrb_value self)               \
  {                                                                           \
    return mrb_bool_value (po_message_is_##field (DATA_PTR (self)));          \
  }
#define BOOL_SETTER(field)                                                    \
  static mrb_value gettextpo_message_m_##field##_set (mrb_state *mrb,         \
                                                      mrb_value self)         \
  {                                                                           \
    bool boolean = true;                                                      \
    mrb_get_args (mrb, "b", &boolean);                                        \
                                                                              \
    po_message_set_##field (DATA_PTR (self), boolean);                        \
    return NIL;                                                               \
  }

BOOL_GETTER (obsolete);
BOOL_SETTER (obsolete);
BOOL_GETTER (fuzzy);
BOOL_SETTER (fuzzy);

#define QUERY_FLAG(name)                                                      \
  static mrb_value gettextpo_message_m_##name##_flag_q (mrb_state *mrb,       \
                                                        mrb_value self)       \
  {                                                                           \
    char *flag = NULL;                                                        \
    mrb_get_args (mrb, "z", &flag);                                           \
                                                                              \
    return mrb_bool_value (                                                   \
        po_message_has_##name##_flag (DATA_PTR (self), flag));                \
  }
#define UPDATE_FLAG(name)                                                     \
  static mrb_value gettextpo_message_m_##name##_flag_update (mrb_state *mrb,  \
                                                             mrb_value self)  \
  {                                                                           \
    char *flag = NULL;                                                        \
    mrb_sym table[] = { MRB_SYM (set) };                                      \
    size_t num = sizeof (table) / sizeof (table[0]);                          \
    mrb_value values[] = { mrb_undef_value () };                              \
    mrb_kwargs kwargs = { .num = num, .table = table, .values = values };     \
    mrb_get_args (mrb, "z:", &flag, &kwargs);                                 \
                                                                              \
    po_message_set_##name##_flag (DATA_PTR (self), flag,                      \
                                  mrb_undef_p (values[0])                     \
                                      || mrb_test (values[0]));               \
    return mrb_nil_value ();                                                  \
  }
#define FLAG_ITERATOR_METHOD(name)                                            \
  static mrb_value gettextpo_message_m_##name##_flag_iterator (               \
      mrb_state *mrb, mrb_value self)                                         \
  {                                                                           \
    return mrb_obj_value (mrb_data_object_alloc (                             \
        mrb, FLAG_ITERATOR,                                                   \
        po_message_##name##_flags_iterator (DATA_PTR (self)),                 \
        &gettextpo_flag_iterator_data_type));                                 \
  }

QUERY_FLAG (workflow);
UPDATE_FLAG (workflow);
FLAG_ITERATOR_METHOD (workflow);

static mrb_value
gettextpo_message_m_format_q (mrb_state *mrb, mrb_value self)
{
  char *format = NULL;
  mrb_get_args (mrb, "z", &format);

  return mrb_bool_value (po_message_is_format (DATA_PTR (self), format));
}

static mrb_value
gettextpo_message_m_format (mrb_state *mrb, mrb_value self)
{
  char *format = NULL;
  mrb_get_args (mrb, "z", &format);

  switch (po_message_get_format (DATA_PTR (self), format))
    {
    case 1:
      return mrb_true_value ();
      break;
    case 0:
      return mrb_false_value ();
      break;
    case -1:
      return mrb_nil_value ();
      break;
    default:
      mrb_raise (mrb, ERROR, "unreachable");
    }
}

static mrb_value
gettextpo_message_m_format_set (mrb_state *mrb, mrb_value self)
{
  char *format = NULL;
  mrb_sym table[] = { MRB_SYM (opposite), MRB_SYM (remove) };
  size_t num = sizeof (table) / sizeof (table[0]);
  mrb_value values[] = { mrb_false_value (), mrb_false_value () };
  mrb_kwargs kwargs = { .num = num, .table = table, .values = values };
  mrb_get_args (mrb, "z:", &format, &kwargs);

  bool opposite = !mrb_undef_p (values[0]) && mrb_test (values[0]);
  bool remove = !mrb_undef_p (values[1]) && mrb_test (values[1]);
  if (opposite && remove)
    mrb_raise (mrb, ERROR,
               "opposite and remove cannot be set at the same time");
  po_message_set_format (DATA_PTR (self), format,
                         opposite ? 0 : (remove ? -1 : 1));
  return NIL;
}

QUERY_FLAG (sticky);
UPDATE_FLAG (sticky);
FLAG_ITERATOR_METHOD (sticky);

static mrb_value
gettextpo_message_m_range (mrb_state *mrb, mrb_value self)
{
  mrb_value range;
  mrb_get_args (mrb, "o", &range);

  int min = mrb_integer (mrb_range_beg (mrb, range));
  int max = mrb_integer (mrb_range_end (mrb, range));
  return mrb_bool_value (po_message_is_range (DATA_PTR (self), &min, &max));
}

static mrb_value
gettextpo_message_m_range_set (mrb_state *mrb, mrb_value self)
{
  mrb_value range;
  mrb_get_args (mrb, "o", &range);

  po_message_set_range (DATA_PTR (self),
                        mrb_integer (mrb_range_beg (mrb, range)),
                        mrb_integer (mrb_range_end (mrb, range)));
  return NIL;
}

static struct
{
  bool error;
  mrb_value *xerror;
  mrb_value *xerror2;
  mrb_state *state;
  mrb_value *file;
} gettextpo_xerror_context = {};

static void
gettextpo_xerror_context_reset (mrb_state *state)
{
  gettextpo_xerror_context.error = false;
  gettextpo_xerror_context.xerror = NULL;
  gettextpo_xerror_context.xerror2 = NULL;
  gettextpo_xerror_context.state = state;
  gettextpo_xerror_context.file = NULL;
}

static void
gettextpo_xerror (const int severity, const po_message_t message,
                  const char *const filename, const size_t lineno,
                  const size_t column, const int multiline_p,
                  const char *const message_text)
{
  gettextpo_xerror_context.error = true;
  if (!gettextpo_xerror_context.xerror
      || mrb_nil_p (*gettextpo_xerror_context.xerror)
      || mrb_undef_p (*gettextpo_xerror_context.xerror))
    return;

  mrb_state *mrb = gettextpo_xerror_context.state;
  mrb_value kwargs = mrb_hash_new (mrb);
  mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (severity)),
                mrb_int_value (mrb, severity));
  if (message)
    {
      mrb_value value = mrb_obj_value (mrb_data_object_alloc (
          mrb, MESSAGE, message, &gettextpo_message_data_type));
      mrb_iv_set (mrb, value, MRB_IVSYM (file),
                  *gettextpo_xerror_context.file);
      mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (message)), value);
    }
  if (filename)
    mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (filename)),
                  mrb_str_new_cstr (mrb, filename));
  if (filename && (lineno != (size_t)(-1)))
    mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (lineno)),
                  mrb_int_value (mrb, lineno));
  if (filename && lineno != (size_t)(-1) && column != (size_t)(-1))
    mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (column)),
                  mrb_int_value (mrb, column));
  mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (multiline)),
                mrb_bool_value (multiline_p));
  if (message_text)
    mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (message_text)),
                  mrb_str_new_cstr (mrb, message_text));
  /* If someday proc calling supports keyword arguments, update
     this. */
  mrb_funcall_id (mrb, *gettextpo_xerror_context.xerror, MRB_SYM (call), 1,
                  kwargs);

  if (severity == PO_SEVERITY_FATAL_ERROR)
    abort ();
}

static void
gettextpo_xerror2 (const int severity, const po_message_t message1,
                   const char *const filename1, const size_t lineno1,
                   const size_t column1, const int multiline_p1,
                   const char *const message_text1,
                   const po_message_t message2, const char *const filename2,
                   const size_t lineno2, const size_t column2,
                   const int multiline_p2, const char *const message_text2)
{
  gettextpo_xerror_context.error = true;
  if (!gettextpo_xerror_context.xerror2
      || mrb_nil_p (*gettextpo_xerror_context.xerror2)
      || mrb_undef_p (*gettextpo_xerror_context.xerror2))
    return;

  mrb_state *mrb = gettextpo_xerror_context.state;
  mrb_value kwargs = mrb_hash_new (mrb);
  mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (severity)),
                mrb_int_value (mrb, severity));
  if (message1)
    {
      mrb_value value = mrb_obj_value (mrb_data_object_alloc (
          mrb, MESSAGE, message1, &gettextpo_message_data_type));
      mrb_iv_set (mrb, value, MRB_IVSYM (file),
                  *gettextpo_xerror_context.file);
      mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (message1)), value);
    }
  if (filename1)
    mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (filename1)),
                  mrb_str_new_cstr (mrb, filename1));
  if (filename1 && (lineno1 != (size_t)(-1)))
    mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (lineno1)),
                  mrb_int_value (mrb, lineno1));
  if (filename1 && lineno1 != (size_t)(-1) && column1 != (size_t)(-1))
    mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (column1)),
                  mrb_int_value (mrb, column1));
  mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (multiline1)),
                mrb_bool_value (multiline_p1));
  if (message_text1)
    mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (message_text1)),
                  mrb_str_new_cstr (mrb, message_text1));
  if (message2)
    {
      mrb_value value = mrb_obj_value (mrb_data_object_alloc (
          mrb, MESSAGE, message2, &gettextpo_message_data_type));
      mrb_iv_set (mrb, value, MRB_IVSYM (file),
                  *gettextpo_xerror_context.file);
      mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (message2)), value);
    }
  if (filename2)
    mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (filename2)),
                  mrb_str_new_cstr (mrb, filename2));
  if (filename2 && (lineno2 != (size_t)(-1)))
    mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (lineno2)),
                  mrb_int_value (mrb, lineno2));
  if (filename2 && lineno2 != (size_t)(-1) && column2 != (size_t)(-1))
    mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (column2)),
                  mrb_int_value (mrb, column2));
  mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (multiline2)),
                mrb_bool_value (multiline_p2));
  if (message_text2)
    mrb_hash_set (mrb, kwargs, mrb_symbol_value (MRB_SYM (message_text2)),
                  mrb_str_new_cstr (mrb, message_text2));
  /* If someday proc calling supports keyword arguments, update
     this. */
  mrb_funcall_id (mrb, *gettextpo_xerror_context.xerror2, MRB_SYM (call), 1,
                  kwargs);

  if (severity == PO_SEVERITY_FATAL_ERROR)
    abort ();
}

static const struct po_xerror_handler gettextpo_xerror_handler = {
  .xerror = gettextpo_xerror,
  .xerror2 = gettextpo_xerror2,
};

static mrb_value
gettextpo_m_formats (mrb_state *mrb, mrb_value self)
{
  mrb_value value = mrb_ary_new (mrb);
  const char *const *formats = po_format_list ();
  for (size_t index = 0; formats[index]; index++)
    mrb_ary_push (mrb, value, mrb_str_new_cstr (mrb, formats[index]));
  return value;
}

static mrb_value
gettextpo_m_format_pretty_name (mrb_state *mrb, mrb_value self)
{
  char *format = NULL;
  mrb_get_args (mrb, "z", &format);
  const char *name = po_format_pretty_name (format);
  return name ? mrb_str_new_cstr (mrb, name) : NIL;
}

static void
gettextpo_file_free (mrb_state *mrb, void *file)
{
  if (file)
    po_file_free (file);
}

static const struct mrb_data_type gettextpo_file_data_type = {
  .struct_name = "gettextpo file",
  .dfree = gettextpo_file_free,
};

static mrb_value
gettextpo_file_m_initialize (mrb_state *mrb, mrb_value self)
{
  mrb_data_init (self, po_file_create (), &gettextpo_file_data_type);
  return self;
}

/**
 * call-seq: read(filename)
 *
 * +filename+ must be resolved.
 */
static mrb_value
gettextpo_file_m_read (mrb_state *mrb, mrb_value klass)
{
  char *filename = NULL;
  mrb_sym table[] = { MRB_SYM (xerror), MRB_SYM (xerror2) };
  size_t num = sizeof (table) / sizeof (table[0]);
  mrb_value values[] = { NIL, NIL };
  mrb_kwargs kwargs = { .num = num, .table = table, .values = values };
  mrb_get_args (mrb, "z:", &filename, &kwargs);

  gettextpo_xerror_context_reset (mrb);
  gettextpo_xerror_context.xerror = &values[0];
  gettextpo_xerror_context.xerror2 = &values[1];
  mrb_value file = mrb_obj_value (mrb_data_object_alloc (
      mrb, mrb_class_ptr (klass), NULL, &gettextpo_file_data_type));
  gettextpo_xerror_context.file = &file;

  po_file_t po_file = po_file_read (filename, &gettextpo_xerror_handler);
  if (gettextpo_xerror_context.error || !po_file)
    mrb_raise (mrb, ERROR, "failed to read file");
  DATA_PTR (file) = po_file;
  return file;
}

static mrb_value
gettextpo_file_m_write (mrb_state *mrb, mrb_value file)
{
  char *filename = NULL;
  mrb_sym table[] = { MRB_SYM (xerror), MRB_SYM (xerror2) };
  size_t num = sizeof (table) / sizeof (table[0]);
  mrb_value values[] = { NIL, NIL };
  mrb_kwargs kwargs = { .num = num, .table = table, .values = values };
  mrb_get_args (mrb, "z:", &filename, &kwargs);

  gettextpo_xerror_context_reset (mrb);
  gettextpo_xerror_context.xerror = &values[0];
  gettextpo_xerror_context.xerror2 = &values[1];
  gettextpo_xerror_context.file = &file;

  po_file_write (DATA_PTR (file), filename, &gettextpo_xerror_handler);
  if (gettextpo_xerror_context.error)
    mrb_raise (mrb, ERROR, "failed to write file");
  return NIL;
}

static mrb_value
gettextpo_file_m_domains (mrb_state *mrb, mrb_value file)
{
  const char *const *domains = po_file_domains (DATA_PTR (file));
  mrb_value value = mrb_ary_new (mrb);
  for (size_t index = 0; domains[index]; index++)
    mrb_ary_push (mrb, value, mrb_str_new_cstr (mrb, domains[index]));
  return value;
}

static mrb_value
gettextpo_file_m_domain_header (mrb_state *mrb, mrb_value file)
{
  char *domain = NULL;
  mrb_get_args (mrb, "|z", &domain);

  const char *header = po_file_domain_header (DATA_PTR (file), domain);
  return header ? mrb_str_new_cstr (mrb, header) : NIL;
}

static void
gettextpo_message_iterator_free (mrb_state *mrb, void *message_iterator)
{
  if (message_iterator)
    po_message_iterator_free (message_iterator);
}

static const struct mrb_data_type gettextpo_message_iterator_data_type = {
  .struct_name = "gettextpo message iterator",
  .dfree = gettextpo_message_iterator_free,
};

static mrb_value
gettextpo_file_m_message_iterator (mrb_state *mrb, mrb_value file)
{
  const char *domain = NULL;
  mrb_get_args (mrb, "|z", &domain);

  mrb_value value = mrb_obj_value (mrb_data_object_alloc (
      mrb,
      mrb_class_get_under_id (mrb,
                              mrb_module_get_id (mrb, MRB_SYM (GettextPO)),
                              MRB_SYM (MessageIterator)),
      po_message_iterator (DATA_PTR (file), domain),
      &gettextpo_message_iterator_data_type));
  mrb_iv_set (mrb, value, MRB_IVSYM (file), file);
  return value;
}

static mrb_value
gettextpo_message_iterator_m_next (mrb_state *mrb, mrb_value message_iterator)
{
  po_message_t message = po_next_message (DATA_PTR (message_iterator));
  if (message)
    {
      mrb_value value = mrb_obj_value (mrb_data_object_alloc (
          mrb,
          mrb_class_get_under_id (mrb,
                                  mrb_module_get_id (mrb, MRB_SYM (GettextPO)),
                                  MRB_SYM (Message)),
          message, &gettextpo_message_data_type));
      mrb_iv_set (mrb, value, MRB_IVSYM (file),
                  mrb_iv_get (mrb, message_iterator, MRB_IVSYM (file)));
      return value;
    }
  else
    mrb_raise (mrb, mrb_class_get_id (mrb, MRB_SYM (StopIteration)),
               "end of PO message iterator");
}

static mrb_value
gettextpo_message_iterator_m_insert (mrb_state *mrb,
                                     mrb_value message_iterator)
{
  char *msgid, *msgstr;
  mrb_get_args (mrb, "zz", &msgid, &msgstr);

  po_message_t message = po_message_create ();
  po_message_set_msgid (message, msgid);
  po_message_set_msgstr (message, msgstr);
  po_message_insert (DATA_PTR (message_iterator), message);

  mrb_value value = mrb_obj_value (mrb_data_object_alloc (
      mrb,
      mrb_class_get_under_id (mrb,
                              mrb_module_get_id (mrb, MRB_SYM (GettextPO)),
                              MRB_SYM (Message)),
      message, &gettextpo_message_data_type));
  mrb_iv_set (mrb, value, MRB_IVSYM (file),
              mrb_iv_get (mrb, message_iterator, MRB_IVSYM (file)));
  return value;
}

static void
gettextpo_filepos_free (mrb_state *mrb, void *filepos)
{
  /* NOTE: filepos is freed by file struct */
}

static const mrb_data_type gettextpo_filepos_data_type = {
  .struct_name = "gettextpo filepos",
  .dfree = gettextpo_filepos_free,
};

static mrb_value
gettextpo_message_m_filepos (mrb_state *mrb, mrb_value self)
{
  mrb_int index = 0;
  mrb_get_args (mrb, "i", &index);

  po_filepos_t pos = po_message_filepos (DATA_PTR (self), index);
  if (pos)
    {
      mrb_value value = mrb_obj_value (mrb_data_object_alloc (
          mrb, mrb_class_get_under_id (mrb, MODULE, MRB_SYM (FilePos)), pos,
          &gettextpo_filepos_data_type));
      mrb_iv_set (mrb, value, MRB_IVSYM (file),
                  mrb_iv_get (mrb, self, MRB_IVSYM (file)));
      return value;
    }
  else
    return NIL;
}

static mrb_value
gettextpo_filepos_m_file (mrb_state *mrb, mrb_value self)
{
  const char *file = po_filepos_file (DATA_PTR (self));
  return file ? mrb_str_new_cstr (mrb, file) : NIL;
}

static mrb_value
gettextpo_filepos_m_start_line (mrb_state *mrb, mrb_value self)
{
  size_t start_line = po_filepos_start_line (DATA_PTR (self));
  return start_line == (size_t)(-1) ? NIL : mrb_int_value (mrb, start_line);
}

static mrb_value
gettextpo_message_m_add_filepos (mrb_state *mrb, mrb_value self)
{
  char *file = NULL;
  size_t start_line = 0;
  mrb_get_args (mrb, "zi", &file, &start_line);

  po_message_add_filepos (DATA_PTR (self), file, start_line);
  return NIL;
}

static mrb_value
gettextpo_message_m_remove_filepos (mrb_state *mrb, mrb_value self)
{
  mrb_int start_line = 0;
  mrb_get_args (mrb, "i", &start_line);

  po_message_remove_filepos (DATA_PTR (self), start_line);
  return NIL;
}

static mrb_value
gettextpo_file_m_check_all (mrb_state *mrb, mrb_value self)
{
  mrb_sym table[] = { MRB_SYM (xerror), MRB_SYM (xerror2) };
  size_t num = sizeof (table) / sizeof (table[0]);
  mrb_value values[] = { NIL, NIL };
  mrb_kwargs kwargs = { .num = num, .table = table, .values = values };
  mrb_get_args (mrb, ":", &kwargs);

  gettextpo_xerror_context_reset (mrb);
  gettextpo_xerror_context.xerror = &values[0];
  gettextpo_xerror_context.xerror2 = &values[1];
  gettextpo_xerror_context.file = &self;

  po_file_check_all (DATA_PTR (self), &gettextpo_xerror_handler);
  if (gettextpo_xerror_context.error)
    mrb_raise (mrb, ERROR, "check all for file failed");
  return NIL;
}

static mrb_value
gettextpo_message_m_check_all (mrb_state *mrb, mrb_value self)
{
  mrb_value iterator;
  mrb_sym table[] = { MRB_SYM (xerror), MRB_SYM (xerror2) };
  size_t num = sizeof (table) / sizeof (table[0]);
  mrb_value values[] = { NIL, NIL };
  mrb_kwargs kwargs = { .num = num, .table = table, .values = values };
  mrb_get_args (mrb, "o:", &iterator, &kwargs);

  gettextpo_xerror_context_reset (mrb);
  gettextpo_xerror_context.xerror = &values[0];
  gettextpo_xerror_context.xerror2 = &values[1];
  mrb_value file = mrb_iv_get (mrb, self, MRB_IVSYM (file));
  gettextpo_xerror_context.file = &file;

  po_message_check_all (DATA_PTR (self), DATA_PTR (iterator),
                        &gettextpo_xerror_handler);
  if (gettextpo_xerror_context.error)
    mrb_raise (mrb, ERROR, "check all for message failed");
  return NIL;
}

static mrb_value
gettextpo_message_m_check_format (mrb_state *mrb, mrb_value self)
{
  mrb_sym table[] = { MRB_SYM (xerror), MRB_SYM (xerror2) };
  size_t num = sizeof (table) / sizeof (table[0]);
  mrb_value values[] = { NIL, NIL };
  mrb_kwargs kwargs = { .num = num, .table = table, .values = values };
  mrb_get_args (mrb, ":", &kwargs);

  gettextpo_xerror_context_reset (mrb);
  gettextpo_xerror_context.xerror = &values[0];
  gettextpo_xerror_context.xerror2 = &values[1];
  mrb_value file = mrb_iv_get (mrb, self, MRB_IVSYM (file));
  gettextpo_xerror_context.file = &file;

  po_message_check_format (DATA_PTR (self), &gettextpo_xerror_handler);
  if (gettextpo_xerror_context.error)
    mrb_raise (mrb, ERROR, "check format for message failed");
  return NIL;
}

void
mrb_mruby_gettextpo_gem_init (mrb_state *const mrb)
{
  struct RClass *const mrb_mGettextPO
      = mrb_define_module_id (mrb, MRB_SYM (GettextPO));
  mrb_define_const_id (mrb, mrb_mGettextPO, MRB_SYM (LIBGETTEXTPO_VERSION),
                       mrb_int_value (mrb, libgettextpo_version));
  mrb_define_const_id (mrb, mrb_mGettextPO, MRB_SYM (SEVERITY_WARNING),
                       mrb_int_value (mrb, PO_SEVERITY_WARNING));
  mrb_define_const_id (mrb, mrb_mGettextPO, MRB_SYM (SEVERITY_ERROR),
                       mrb_int_value (mrb, PO_SEVERITY_ERROR));
  mrb_define_const_id (mrb, mrb_mGettextPO, MRB_SYM (SEVERITY_FATAL_ERROR),
                       mrb_int_value (mrb, PO_SEVERITY_FATAL_ERROR));
  mrb_define_module_function_id (mrb, mrb_mGettextPO, MRB_SYM (formats),
                                 gettextpo_m_formats, MRB_ARGS_NONE ());
  mrb_define_module_function_id (
      mrb, mrb_mGettextPO, MRB_SYM (format_pretty_name),
      gettextpo_m_format_pretty_name, MRB_ARGS_REQ (1));
  mrb_define_module_function_id (
      mrb, mrb_mGettextPO, MRB_SYM (header_entry_value),
      gettextpo_m_header_entry_value, MRB_ARGS_REQ (2));
  mrb_define_module_function_id (
      mrb, mrb_mGettextPO, MRB_SYM (header_with_updated_entry_value),
      gettextpo_m_header_with_updated_entry_value, MRB_ARGS_REQ (3));
  struct RClass *const mrb_cFile = mrb_define_class_under_id (
      mrb, mrb_mGettextPO, MRB_SYM (File), mrb->object_class);
  MRB_SET_INSTANCE_TT (mrb_cFile, MRB_TT_CDATA);
  mrb_define_class_method_id (mrb, mrb_cFile, MRB_SYM (read),
                              gettextpo_file_m_read, MRB_ARGS_ANY ());
  mrb_define_method_id (mrb, mrb_cFile, MRB_SYM (initialize),
                        gettextpo_file_m_initialize, MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cFile, MRB_SYM (write),
                        gettextpo_file_m_write, MRB_ARGS_ANY ());
  mrb_define_method_id (mrb, mrb_cFile, MRB_SYM (domains),
                        gettextpo_file_m_domains, MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cFile, MRB_SYM (message_iterator),
                        gettextpo_file_m_message_iterator, MRB_ARGS_OPT (1));
  mrb_define_method_id (mrb, mrb_cFile, MRB_SYM (domain_header),
                        gettextpo_file_m_domain_header, MRB_ARGS_OPT (1));
  mrb_define_method_id (mrb, mrb_cFile, MRB_SYM (check_all),
                        gettextpo_file_m_check_all, MRB_ARGS_ANY ());
  struct RClass *const mrb_cMessage = mrb_define_class_under_id (
      mrb, mrb_mGettextPO, MRB_SYM (Message), mrb->object_class);
  MRB_SET_INSTANCE_TT (mrb_cMessage, MRB_TT_CDATA);
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (msgctxt),
                        gettextpo_message_m_msgctxt, MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_E (msgctxt),
                        gettextpo_message_m_msgctxt_set, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (msgid),
                        gettextpo_message_m_msgid, MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_E (msgid),
                        gettextpo_message_m_msgid_set, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (msgid_plural),
                        gettextpo_message_m_msgid_plural, MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_E (msgid_plural),
                        gettextpo_message_m_msgid_plural_set,
                        MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (msgstr),
                        gettextpo_message_m_msgstr, MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_E (msgstr),
                        gettextpo_message_m_msgstr_set, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (msgstr_plural),
                        gettextpo_message_m_msgstr_plural, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (set_msgstr_plural),
                        gettextpo_message_m_msgstr_plural_set,
                        MRB_ARGS_REQ (2));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (comments),
                        gettextpo_message_m_comments, MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_E (comments),
                        gettextpo_message_m_comments_set, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (extracted_comments),
                        gettextpo_message_m_extracted_comments,
                        MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_E (extracted_comments),
                        gettextpo_message_m_extracted_comments_set,
                        MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (prev_msgctxt),
                        gettextpo_message_m_prev_msgctxt, MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_E (prev_msgctxt),
                        gettextpo_message_m_prev_msgctxt_set,
                        MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (prev_msgid),
                        gettextpo_message_m_prev_msgid, MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_E (prev_msgid),
                        gettextpo_message_m_prev_msgid_set, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (prev_msgid_plural),
                        gettextpo_message_m_prev_msgid_plural,
                        MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_E (prev_msgid_plural),
                        gettextpo_message_m_prev_msgid_plural_set,
                        MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_Q (obsolete),
                        gettextpo_message_m_obsolete, MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_E (obsolete),
                        gettextpo_message_m_obsolete_set, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_Q (fuzzy),
                        gettextpo_message_m_fuzzy, MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_E (fuzzy),
                        gettextpo_message_m_fuzzy_set, MRB_ARGS_REQ (1));

  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_Q (workflow_flag),
                        gettextpo_message_m_workflow_flag_q, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (update_workflow_flag),
                        gettextpo_message_m_workflow_flag_update,
                        MRB_ARGS_ANY ());
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (workflow_flag_iterator),
                        gettextpo_message_m_workflow_flag_iterator,
                        MRB_ARGS_NONE ());

  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_Q (format),
                        gettextpo_message_m_format_q, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (format),
                        gettextpo_message_m_format, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (update_format),
                        gettextpo_message_m_format_set, MRB_ARGS_ANY ());

  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_Q (sticky_flag),
                        gettextpo_message_m_sticky_flag_q, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (update_sticky_flag),
                        gettextpo_message_m_sticky_flag_update,
                        MRB_ARGS_ANY ());
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (sticky_flag_iterator),
                        gettextpo_message_m_sticky_flag_iterator,
                        MRB_ARGS_NONE ());

  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_Q (range),
                        gettextpo_message_m_range, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM_E (range),
                        gettextpo_message_m_range_set, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (filepos),
                        gettextpo_message_m_filepos, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (add_filepos),
                        gettextpo_message_m_add_filepos, MRB_ARGS_REQ (2));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (remove_filepos),
                        gettextpo_message_m_remove_filepos, MRB_ARGS_REQ (1));
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (check_all),
                        gettextpo_message_m_check_all, MRB_ARGS_ANY ());
  mrb_define_method_id (mrb, mrb_cMessage, MRB_SYM (check_format),
                        gettextpo_message_m_check_format, MRB_ARGS_ANY ());
  struct RClass *const mrb_cMessageIterator = mrb_define_class_under_id (
      mrb, mrb_mGettextPO, MRB_SYM (MessageIterator), mrb->object_class);
  MRB_SET_INSTANCE_TT (mrb_cMessageIterator, MRB_TT_CDATA);
  mrb_define_method_id (mrb, mrb_cMessageIterator, MRB_SYM (next),
                        gettextpo_message_iterator_m_next, MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cMessageIterator, MRB_SYM (insert),
                        gettextpo_message_iterator_m_insert, MRB_ARGS_REQ (2));
  struct RClass *const mrb_cFilePos = mrb_define_class_under_id (
      mrb, mrb_mGettextPO, MRB_SYM (FilePos), mrb->object_class);
  MRB_SET_INSTANCE_TT (mrb_cFilePos, MRB_TT_CDATA);
  mrb_define_method_id (mrb, mrb_cFilePos, MRB_SYM (file),
                        gettextpo_filepos_m_file, MRB_ARGS_NONE ());
  mrb_define_method_id (mrb, mrb_cFilePos, MRB_SYM (start_line),
                        gettextpo_filepos_m_start_line, MRB_ARGS_NONE ());
  struct RClass *const mrb_cFlagIterator = mrb_define_class_under_id (
      mrb, mrb_mGettextPO, MRB_SYM (FlagIterator), mrb->object_class);
  MRB_SET_INSTANCE_TT (mrb_cFlagIterator, MRB_TT_CDATA);
  mrb_define_method_id (mrb, mrb_cFlagIterator, MRB_SYM (next),
                        gettextpo_flag_iterator_m_next, MRB_ARGS_NONE ());
  DONE;
}

void
mrb_mruby_gettextpo_gem_final (const mrb_state *const mrb)
{
}
