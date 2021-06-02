#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>

#include "libpmemobj.h"

#include "layouts.h"

int main( int argc, char *argv[] )
{
  struct t1 t;

  PMEMobjpool *ppool = nullptr;
  TOID(struct pmem_root) rp;
  
  if( not std::filesystem::exists( argv[1] ) ) {
    perror( "no filename provided" );
    return 1;
  }
  
  ppool = pmemobj_open( argv[1], POBJ_LAYOUT_NAME(cereal_layout) );
  if( ppool == nullptr ) {
    perror( "pmemobj open" );
    return 1;
  }

  rp = POBJ_ROOT( ppool, struct pmem_root );
  std::cout << "root nobj is " << D_RO(rp)->nobj << std::endl;
  TOID(struct lenbuf) lbo = D_RO(rp)->data;
  std::string str ( D_RO( D_RO(lbo)->buf ), D_RO(lbo)-> len );
  std::istringstream ss( str, std::ios_base::binary );
  cereal::PortableBinaryInputArchive iarch( ss );
  iarch( t );

  std::cout << "struct t1 contains "
	    << t.a << ","
	    << t.b << ","
	    << t.c << std::endl;

  pmemobj_close( ppool );
  return 0;
}
