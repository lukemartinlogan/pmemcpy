#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <filesystem>

#include <chrono>
#include <random>

#include "libpmemobj.h"

#include "layouts.h"


int
lenbuf_ctor( PMEMobjpool* pool, void* ptr, void* arg )
{
  struct lenbuf2* lb2 = static_cast< struct lenbuf2* >( ptr );
  std::ostringstream* ssp = static_cast< std::ostringstream* >( arg );

  // assign and persist the length of the buffer
  lb2->len = ssp->str().size();
  pmemobj_persist( pool, &lb2->len, sizeof( size_t ) );

  // allocate the buffer and copy in using pmemobj_memcpy_persist to mark it
  lb2->buf = static_cast< char* >( std::malloc( lb2->len ) );
  pmemobj_memcpy_persist( pool, lb2->buf, ssp->str().data(), lb2->len );

  return 0;
}
     
  

int main( int argc, char *argv[] )
{
  std::random_device rd;
  struct t1 t;
 
  std::mt19937 generator( rd() );
  std::uniform_int_distribution< uint32_t > distribute( 1, 1000000 );
  std::uniform_real_distribution< double > distribute_real( 1.0, 1000000.0 );

  PMEMobjpool *ppool = nullptr;
  TOID(struct pmem_root2) rp;

  if (not std::filesystem::exists( argv[1] ) ) {
    ppool = pmemobj_create( argv[1], POBJ_LAYOUT_NAME(c2_layout), PMEMOBJ_MIN_POOL, 0666 );
    if( ppool == nullptr ) {
      perror( "pmemobj create" );
      return 1;
    }
  } else {
    ppool = pmemobj_open( argv[1], POBJ_LAYOUT_NAME(c2_layout) );
    if( ppool == nullptr ) {
      perror( "pmemobj open" );
      return 1;
    }
  }
  
  rp = POBJ_ROOT( ppool, struct pmem_root2 );
  D_RW(rp)->nobj = 98;
  
  for( int i = 0; i < 5; i++ ) {
    std::ostringstream ss( std::ios_base::binary );
    cereal::BinaryOutputArchive arch( ss );
    struct t1 t = { distribute( generator ), distribute( generator ), distribute_real( generator ) };
    arch( t );
    
    std::cout << "struct t1 contains " << t << std::endl;
    
    POBJ_LIST_INSERT_NEW_TAIL( ppool,
			       &D_RW( rp )->head, chunks,
			       sizeof( struct lenbuf2 ),
			       lenbuf_ctor,
			       &ss );
  }

  std::cout << "root nobj is " << D_RO(rp)->nobj << std::endl;

  std::cout << "closing." << std::endl;
  pmemobj_close( ppool );

  std::cout << "reopening." << std::endl;
  ppool = pmemobj_open( argv[1], POBJ_LAYOUT_NAME(c2_layout) );
  rp = POBJ_ROOT( ppool, struct pmem_root2 );
  std::cout << "magic number "
	    << (( D_RW(rp)->nobj == 98 ) ? "matches" : "does not match" )
	    << std::endl;

  std::cout << "iterating..." << std::endl;

  TOID(struct lenbuf2) iter;
  POBJ_LIST_FOREACH( iter, &D_RO(rp)->head, chunks ) {
    struct t1 t;
    std::string str( D_RO(iter)->buf, D_RO(iter)->len );
    std::istringstream iss( str, std::ios_base::binary );
    cereal::BinaryInputArchive iarch( iss );
    iarch( t );
    std::cout << "struct t1 contains " << t << std::endl;
    iss.str().clear();
  }

  std::cout << "closing." << std::endl;
  pmemobj_close( ppool );

  return 0;
}
