/*
    This file is part of VK/KittenPHP-DB-Engine.

    VK/KittenPHP-DB-Engine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    VK/KittenPHP-DB-Engine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VK/KittenPHP-DB-Engine.  If not, see <http://www.gnu.org/licenses/>.

    This program is released under the GPL with the additional exemption
    that compiling, linking, and/or using OpenSSL is allowed.
    You are free to remove this exemption from derived works.

    Copyright 2011-2013 Vkontakte Ltd
              2011-2013 Vitaliy Valtman
*/

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "vkext_flex.h"
#include "vkext_flex_auto.c"
#include "php.h"

#define BUFF_LEN (1 << 16)
static char buff[BUFF_LEN];

char *vk_estrdup (const char *s) {
    char *d = emalloc (strlen (s) + 1);
    if (d == NULL) return NULL;
    strcpy (d,s);
    return d;
}

char *do_flex (const char *name, int name_len, const char *case_name, int case_name_len, int sex, const char *type, int lang_id) {
  if (name_len  > (1 << 10)) {
    return vk_estrdup (name);
  }
  struct lang *cur_lang;
  if (lang_id < 0 || lang_id >= LANG_NUM || !langs[lang_id]) {
    return vk_estrdup (name);
  }
  cur_lang = langs[lang_id];
  assert (cur_lang);
  int t = -1;
  if (!strcmp (type, "names")) {
    if (cur_lang->names_start < 0) {
      return vk_estrdup (name);
    }
    t = cur_lang->names_start;
  } else if (!strcmp (type, "surnames")) {
    if (cur_lang->surnames_start < 0) {
      return vk_estrdup (name);
    }
    t = cur_lang->surnames_start;
  } else {
    return vk_estrdup (name);
  }
  assert (t >= 0);
  if (sex != 1) {
    sex = 0;
  }
  int ca = -1;
  int i;
  for (i = 0; i < CASES_NUM; i++) if (!strcmp (cases_names[i], case_name)) {
    ca = i;
    break;
  }
  if (ca == -1 || ca >= cur_lang->cases_num) {
    return vk_estrdup (name);
  }
  assert (ca >= 0 && ca < cur_lang->cases_num);

  int p = 0;
  int wp = 0;
  while (p < name_len) {
    int pp = p;
    while (pp < name_len && name[pp] != '-') {
      pp++;
    }
    int hyphen = (name[pp] == '-');
    int tt = t;
    int best = -1;
    int save_pp = pp;
    int new_tt;
    int isf = 0;
    if (pp - p > 0) {
      const char *fle = cur_lang->flexible_symbols;
      while (*fle) {
        if (*fle == name[pp - 1]) {
          isf = 1;
          break;
        }
        fle ++;
      }
    }
    while (1 && isf) {
      assert (tt >= 0);
      if (cur_lang->nodes[tt].tail_len >= 0 && (!cur_lang->nodes[tt].hyphen || hyphen)) {
        best = tt;
      }
      unsigned char c;
      if (pp == p - 1) {
        break;
      }
      pp --;
      if (pp < p) {
        c = 0;
      } else {
        c = name[pp];
      }
      new_tt = -1;
      int l = cur_lang->nodes[tt].children_start;
      int r = cur_lang->nodes[tt].children_end;
      if (r - l <= 4) {
        for (i = l; i < r; i++) if (cur_lang->children[2 * i] == c) {
          new_tt = cur_lang->children[2 * i + 1] ;
          break;
        }
      } else {
        int x;
        while (r - l > 1) {
          x = (r + l) >> 1;
          if (cur_lang->children[2 * x] <= c) {
            l = x;
          } else {
            r = x;
          }
        }
        if (cur_lang->children[2 * l] == c) {
          new_tt = cur_lang->children[2 * l + 1];
        }
      }
      if (new_tt == -1) {
        break;
      } else {
        tt = new_tt;
      }
    }
    if (best == -1) {
      memcpy (buff + wp, name + p, save_pp - p);
      wp += (save_pp - p);
    } else {
      int r = -1;
      if (!sex) {
        r = cur_lang->nodes[best].male_endings;
      } else {
        r = cur_lang->nodes[best].female_endings;
      }
      if (r < 0 || !cur_lang->endings[r * cur_lang->cases_num + ca]) {
        memcpy (buff + wp, name + p, save_pp - p);
        wp += (save_pp - p);
      } else {
        int ml = save_pp - p - cur_lang->nodes[best].tail_len;
        if (ml < 0) {
          ml = 0;
        }
        memcpy (buff + wp, name + p, ml);
        wp += ml;
        strcpy (buff + wp, cur_lang->endings[r * cur_lang->cases_num + ca]);
        wp += strlen (cur_lang->endings[r * cur_lang->cases_num + ca]);
      }
    }
    if (hyphen) {
      buff[wp++] = '-';
    } else {
      buff[wp++] = 0;
    }
    p = save_pp + 1;
  }

  return vk_estrdup (buff);
}
