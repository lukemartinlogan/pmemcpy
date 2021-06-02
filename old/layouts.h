#ifndef LAYOUTS_H
#define LAYOUTS_H

#pragma once

#include "cereal/types/memory.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/archives/portable_binary.hpp"

#include "libpmemobj.h"

struct t1
{
  uint32_t a, b;
  double c;

  template< typename Archive >
  void serialize( Archive &ar )
  {
    ar( a, b, c );
  }
};

std::ostream&
operator<<( std::ostream& os, const struct t1& t )
{
  os << "( " << t.a
     << ","  << t.b
     << ","  << t.c
     << " )";
  return os;
}


POBJ_LAYOUT_BEGIN(cereal_layout);
POBJ_LAYOUT_ROOT(cereal_layout, struct pmem_root);
POBJ_LAYOUT_TOID(cereal_layout, struct lenbuf);
POBJ_LAYOUT_TOID(cereal_layout, char);
POBJ_LAYOUT_END(cereal_layout);


struct lenbuf {
  size_t len;
  TOID(char) buf;
};

struct pmem_root {
  size_t nobj;
  TOID(struct lenbuf) data;
};

POBJ_LAYOUT_BEGIN(c2_layout);
POBJ_LAYOUT_ROOT(c2_layout, struct pmem_root2);
POBJ_LAYOUT_TOID(c2_layout, struct lenbuf2);
POBJ_LAYOUT_END(c2_layout);

struct lenbuf2 {
  POBJ_LIST_ENTRY(struct lenbuf2) chunks;
  size_t len;
  char *buf;
};

struct pmem_root2 {
  size_t nobj;
  POBJ_LIST_HEAD(plist, struct lenbuf2) head;
};


#endif
