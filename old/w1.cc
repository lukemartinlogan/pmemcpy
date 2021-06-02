#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <filesystem>

#include <chrono>
#include <random>

#include "libpmemobj.h"

#include "layouts.h"


int main( int argc, char *argv[] )
{
  std::random_device rd;
  std::stringstream ss( std::ios_base::in | std::ios_base::out | std::ios_base::binary );
  struct t1 t;

  std::mt19937 generator( rd() );
  std::uniform_int_distribution< uint32_t > distribute( 1, 1000000 );
  std::uniform_real_distribution< double > distribute_real( 1.0, 1000000.0 );
  
  t.a = distribute( generator );
  t.b = distribute( generator );
  t.c = distribute_real( generator );

  std::cout << "struct t1 contains "
	    << t.a << ","
	    << t.b << ","
	    << t.c << std::endl;

  cereal::PortableBinaryOutputArchive arch( ss );
  arch( t );




  PMEMobjpool *ppool = nullptr;
  TOID(struct pmem_root) rp;
  
  if( not std::filesystem::exists( argv[1] ) ) {
    ppool = pmemobj_create( argv[1], POBJ_LAYOUT_NAME(cereal_layout), PMEMOBJ_MIN_POOL, 0666 );
    if( ppool == nullptr ) {
      perror( "pmemobj create" );
      return 1;
    }
  } else {
    ppool = pmemobj_open( argv[1], POBJ_LAYOUT_NAME(cereal_layout) );
    if( ppool == nullptr ) {
      perror( "pmemobj open" );
      return 1;
    }
  }

  // allocate the pmem buf

  rp = POBJ_ROOT( ppool, struct pmem_root );

  TX_BEGIN(ppool) {
    TX_ADD( rp );
    D_RW(rp)->nobj = 99;
    
    TOID(struct lenbuf) lbo = TX_NEW(struct lenbuf);
    size_t sbuflen = ss.str().size();
    D_RW(lbo)->len = sbuflen;
    D_RW(lbo)->buf = TX_ZALLOC( char, sbuflen );
    std::memcpy( D_RW( D_RW(lbo)->buf ), ss.str().data(), sbuflen );
    D_RW(rp)->data = lbo;
  } TX_ONABORT {
    std::cerr << "TX aborted" << std::endl;
  } TX_ONCOMMIT {
    std::cerr << "TX commit" << std::endl;
  } TX_END 
      ;

  std::cout << "root nobj is " << D_RO(rp)->nobj << std::endl;
      
  pmemobj_close( ppool );
  return 0;
}
