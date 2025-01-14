////////////////////////////////////////////////////////////////////////
// Class:       VDColdboxTDEChannelMapService
// File:        VDColdboxTDEChannelMapService_service.cc
// Author:      Vyacheslav Galymov
//
// Mappings for VD CRP1 from docdb 23910
// Mappings for VD CRP2 from docdb 25847
// 
// Classes to facility channel order translation between different 
// representations
// 
// seqn   - unique counter to give channel data position in the record
// crate  - utca crate number starting
// card   - AMC card number
// cardch - AMC card channel number 
// crp    - CRP number
// view   - view number (0/1/2)
// viewch - view channel number 
// 
// Boost multi_index_container provides interface to search and order various 
// indicies. The container structure is defined in TDEChannelTable, which has
// the following interfaces:
//  - raw sequence index, tag IndexRawSeqn
//  - crate number, tag IndexCrate, to get all channels to a given crate
//  - crate number and card number, tag IndexCrateCard, 
//    to get all channels for a given card in a crate
//  - crate, card, and channel number, tag IndexCrateCardChan, to access
//    a given channel of a given card in a given crate
//  - CRP index, tag IndexCrp, to access channels assigned to a given CRP
//  - CRP index and view index, tag IndexCrpView, to access channels assigned to 
//    a given view in a specified CRP
//  - CRP index, view index, and channel number, tag IndexCrpViewChan, to access 
//    a given view channel in a given CRP
//
// Modified:
//   VG Mon Sep  5 14:47:39 CEST 2022: added CB CRP2 channel map based
//                                     on docdb-25847
// 
////////////////////////////////////////////////////////////////////////

// framework libraries
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib_except/exception.h" // cet::exception

// boost
#include <boost/range.hpp>
#include <boost/range/adaptors.hpp>

// c++
#include <iostream>
#include <iomanip>
#include <utility>

#include "VDColdboxTDEChannelMapService.h"
#include "tde_cmap_utils.h"
#include "kel_chan_map.h"

using dune::tde::ChannelId;
using std::vector;

// ctor
dune::VDColdboxTDEChannelMapService::VDColdboxTDEChannelMapService(fhicl::ParameterSet const& p,
								   art::ActivityRegistry& areg)
{
  mapname_  = "";
  ntot_     = 0;
  ncrates_  = 0;
  ncrps_    = 0;
  nch_      = 64;

  std::string MapName  = p.get<std::string>("MapName", "vdcb1crp");
  unsigned ncrateInMap = p.get<unsigned>("MapCrateNb", 3);
  unsigned ncardsInMap = p.get<unsigned>("MapCardNb", 10);
  unsigned nviewsInMap = p.get<unsigned>("MapViewNb",  1);
  fLogLevel            = p.get<int>("LogLevel", 0);

  if( fLogLevel ){
    std::cout<<"VDColdboxTDEChannelMapService::ctor: MapName : "<<MapName<<std::endl;
  }

  //initialize channel map
  initMap( MapName, ncrateInMap, ncardsInMap, nviewsInMap );

  if( fLogLevel >= 3){
    auto all_chans = find_by_seqn(0, ntot());
    print( all_chans );
  }
}


// 
// initMap
void dune::VDColdboxTDEChannelMapService::initMap( std::string mapname, unsigned ncrates,
						    unsigned ncards, unsigned nviews )
{
  // already defined?
  if( mapname_.compare( mapname ) == 0 ) {
    return;
  }
  
  clearMap();
  mapname_ = mapname;
  if( mapname.compare("vdcb1crp") == 0 ) {
    vdcb1crpMap();
  } 
  else if( mapname.compare("vdcb2crp") == 0 ) {
    vdcb2crpMap();
  }
  else {
    simpleMap( ncrates, ncards, nviews );
  }
}


//
// clearMap 
void dune::VDColdboxTDEChannelMapService::clearMap()
{
  tde::ChannelTable().swap( chanTable );
  ncrates_ = 0;
  ncrps_   = 0;
  ntot_    = 0;
  mapname_ = "";
  crateidx_.clear();
  crpidx_.clear();
}


//
// a simple channel map for testing purposes
void dune::VDColdboxTDEChannelMapService::simpleMap( unsigned ncrates, unsigned ncards, 
						      unsigned nviews )
{
  unsigned nctot  = ncards * ncrates; // total number of cards
  unsigned ncview = nctot / nviews;   // allocate the same for each view
  unsigned nch    = nch_;             // number of ch per card (fixed)
  
  unsigned seqn  = 0;
  unsigned crate = 0;
  unsigned crp   = 0;
  unsigned view  = 0;
  unsigned vch   = 0;
  for( unsigned card = 0; card < nctot; card++ )
    {
      if( card > 0 ) {
	if( card % ncards == 0 ) crate++;
	if( card % ncview == 0 ) {view++; vch=0;}
      }
      for( unsigned ch = 0; ch < nch; ch++ ){
	add( seqn++, crate, card % ncards, ch, crp, view, vch++);
      }
    }
  //
}



//
// add channl ID to map
void dune::VDColdboxTDEChannelMapService::add( unsigned seq, unsigned crate, unsigned card, 
					      unsigned cch,  unsigned crp, unsigned view, 
					      unsigned vch, unsigned short state )
{
  chanTable.insert( ChannelId(seq, crate, card, cch, crp, view, vch, state) );
  //
  ntot_    = chanTable.size();

  crateidx_.insert( crate );
  ncrates_ = crateidx_.size();
      
  crpidx_.insert( crp );
  ncrps_ = crpidx_.size();
}

//
// 
//
boost::optional<ChannelId> dune::VDColdboxTDEChannelMapService::find_by_seqn( unsigned seqn ) const
{
  auto it = chanTable.get<dune::tde::IndexRawSeqnHash>().find( seqn );
  if( it != chanTable.get<dune::tde::IndexRawSeqnHash>().end() )
    return *it;
  
  return boost::optional<ChannelId>();
}

//
// the most low level info
std::vector<ChannelId> dune::VDColdboxTDEChannelMapService::find_by_seqn( unsigned from, unsigned to ) const
{
  if( to < from ) std::swap( from, to );
  
  std::vector<ChannelId> res;
  
  if( from == to )
    {
      if( boost::optional<ChannelId> id = find_by_seqn( from ) ) 
	res.push_back( *id );
      //auto it = chanTable.find(from);
      //if( it != chanTable.end() )
      //res.push_back( *it );
    }
  else
    {
      auto first = chanTable.get<dune::tde::IndexRawSeqn>().lower_bound( from );
      auto last  = chanTable.get<dune::tde::IndexRawSeqn>().upper_bound( to );
      res.insert( res.begin(), first, last );
    }
  
  return res;
}

//
//
std::vector<ChannelId> dune::VDColdboxTDEChannelMapService::find_by_crate( unsigned crate, bool ordered ) const
{
  if( not ordered ) // get from hashed index
    {
      const auto r = chanTable.get<dune::tde::IndexCrate>().equal_range( crate );
      std::vector<ChannelId> res(r.first, r.second);
      return res;
    }

  const auto r = chanTable.get<dune::tde::IndexCrateCardChan>().equal_range( boost::make_tuple(crate) );
  std::vector<ChannelId> res(r.first, r.second);

  return res;
}

//
//
std::vector<ChannelId> dune::VDColdboxTDEChannelMapService::find_by_crate_card( unsigned crate, unsigned card, bool ordered ) const
{
  if( not ordered ) // get from hashed index
    {
      const auto r = chanTable.get<dune::tde::IndexCrateCard>().equal_range( boost::make_tuple(crate, card) );
      std::vector<ChannelId> res(r.first, r.second);
      return res;
    }
  
  // ordered accodring to channel number
  const auto r = chanTable.get<dune::tde::IndexCrateCardChan>().equal_range( boost::make_tuple( crate, card) );
  std::vector<ChannelId> res(r.first, r.second);
  
  return res;
}

//
//
boost::optional<ChannelId> dune::VDColdboxTDEChannelMapService::find_by_crate_card_chan( unsigned crate,
								unsigned card, unsigned chan ) const
{
  auto it = chanTable.get<dune::tde::IndexCrateCardChanHash>().find( boost::make_tuple(crate, card, chan) );
  if( it != chanTable.get<dune::tde::IndexCrateCardChanHash>().end() )
    return *it;
  
  return boost::optional<ChannelId>();
}

//
//
std::vector<ChannelId> dune::VDColdboxTDEChannelMapService::find_by_crp( unsigned crp, bool ordered ) const
{
  if( not ordered ) // get from hashed index
    {
      const auto r = chanTable.get<dune::tde::IndexCrp>().equal_range( crp );
      std::vector<ChannelId> res(r.first, r.second);
      return res;
    }

  const auto r = chanTable.get<dune::tde::IndexCrpViewChan>().equal_range( crp );
  std::vector<ChannelId> res(r.first, r.second);
  //return res;
  return res;
}

//
//
std::vector<ChannelId> dune::VDColdboxTDEChannelMapService::find_by_crp_view( unsigned crp, unsigned view, bool ordered ) const
{
  if( not ordered ) // get from hashed index
    {
      const auto r = chanTable.get<dune::tde::IndexCrpView>().equal_range( boost::make_tuple(crp, view) );
      std::vector<ChannelId> res(r.first, r.second);
      return res;
    }
  
  // ordered accodring to channel number
  const auto r = chanTable.get<dune::tde::IndexCrpViewChan>().equal_range( boost::make_tuple(crp, view) );
  std::vector<ChannelId> res(r.first, r.second);
  return res;
}

//
//
boost::optional<ChannelId> dune::VDColdboxTDEChannelMapService::find_by_crp_view_chan( unsigned crp,
											unsigned view, unsigned chan ) const
{
  auto it = chanTable.get<dune::tde::IndexCrpViewChanHash>().find( boost::make_tuple(crp, view, chan) );
  if( it != chanTable.get<dune::tde::IndexCrpViewChanHash>().end() )
    return *it;
  
  return boost::optional<ChannelId>();
}
  
//
// Map to CRP channels
int dune::VDColdboxTDEChannelMapService::MapToCRP(int seqch, int &crp, int &view, int &chv) const
{
  crp = view = chv = -1;
  if( boost::optional<ChannelId> id = find_by_seqn( (unsigned)seqch ) ) 
    {
      if( !id->exists() ) return -1;
      crp  = id->crp();
      view = id->view();
      chv  = id->viewch();
      return 1;
    }
  
  return -1;
}

//
// Map to DAQ channel sequence
int dune::VDColdboxTDEChannelMapService::MapToDAQ(int crp, int view, int chv, int &seqch) const
{
  seqch = -1;
  if( boost::optional<ChannelId> id = find_by_crp_view_chan( (unsigned)crp, (unsigned)view, (unsigned)chv ) )
    {
      if( !id->exists() ) return -1;
      seqch = id->seqn();
      return 1;
    }
  return -1;
}

//
// number of cards assigned to a given crate
unsigned dune::VDColdboxTDEChannelMapService::ncards( unsigned crate ) const
{
  // assumes it is sorted according to card number
  unsigned count  = 0;
  auto r = chanTable.get<dune::tde::IndexCrateCardChan>().equal_range( crate );
  ssize_t last    = -1;
  for( ChannelId const &ch : boost::make_iterator_range( r ) )
    {
      if( !ch.exists() ) continue;
      unsigned val = ch.card();
      if( val != last ) 
	{
	  count++;
	  last = val;
	}
    }
  
  return count;
}

//
// number of views assigned to a given CRP
unsigned dune::VDColdboxTDEChannelMapService::nviews( unsigned crp ) const 
{
  // assumes it is sorted according to crp number
  unsigned count  = 0;
  auto r = chanTable.get<dune::tde::IndexCrpViewChan>().equal_range( crp );
  ssize_t last    = -1;
  for( ChannelId const &ch : boost::make_iterator_range( r ) )
    {
      if( !ch.exists() ) continue;
      unsigned val = ch.view();
      if( val != last ) 
	{
	  count++;
	  last = val;
	}
    }
  
  return count;
}

//
//
void dune::VDColdboxTDEChannelMapService::print( std::vector<ChannelId> &vec )
{
  for( auto it = vec.begin();it!=vec.end();++it )
    {
      unsigned seqn   = it->seqn();
      unsigned crate  = it->crate();
      unsigned card   = it->card();
      unsigned cch    = it->cardch();
      unsigned crp    = it->crp();
      unsigned view   = it->view();
      unsigned vch    = it->viewch();
      unsigned state  = it->state();
      bool     exists = it->exists();
      std::cout<<std::setw(7)<<seqn
	       <<std::setw(4)<<crate
	       <<std::setw(3)<<card
	       <<std::setw(3)<<cch
	       <<std::setw(3)<<crp
	       <<std::setw(2)<<view
	       <<std::setw(6)<<vch
	       <<std::setw(2)<<state
	       <<std::setw(2)<<exists<<std::endl;
    }
}

// CRP for 1st coldbox test
void dune::VDColdboxTDEChannelMapService::vdcb1crpMap(){

  //int ncrates = 3;
  int nslots  = 10;
  int nview   = 3;

  // utca crate 1
  dune::tde::crate c1( 0, nslots );
  vector<int> c1_kel{34, 36, 35, 38, 37, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49};
  c1.add_crp_connection( 0, 0, c1_kel );

  // utca crate 2
  dune::tde::crate c2( 1, nslots );
  vector<int> c2_kel{14, 16, 15, 18, 17, 19, 20, 22, 21, 25, 23, 24, 26, 27, 28, 32, 29, 30, 31, 33};
  c2.add_crp_connection( 0, 0, c2_kel );

  // utca crate 3
  dune::tde::crate c3( 2, nslots );
  vector<int> c3_kel{0, 2, 1, 5, 3, 4, 6, 7, 8, 12, 9, 10, 11, 13};
  c3.add_crp_connection( 0, 0, c3_kel );
  
  vector<dune::tde::crate> crates{c1, c2, c3};

  // all connector mappings should include ADC channel inversion on AMCs 
  // the inversion is in group of 8ch: AMC ch 0 -> 7 should be remapped to 7 -> 0
  
  // kel connector orientation in a given view, chans 0 -> 31
  vector<unsigned> kel_nor = { 7,  6,  5,  4,  3,  2,  1,  0, 15, 14, 13, 12, 11, 10,  9,
			       8, 23, 22, 21, 20, 19, 18, 17, 16, 31, 30, 29, 28, 27, 26, 25, 24 };
  // kel connector orientation in a given view, chans 31 -> 0
  vector<unsigned> kel_inv = {24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22,
			      23,  8,  9, 10, 11, 12, 13, 14, 15,  0,  1,  2,  3,  4,  5,  6,  7 };

  
  // kel connectors for each view sorted in the view channel order
  // induction 1
  vector<int> kel_view0{1,5,8,12,15,18,21,25,28,32,35,38};
  std::reverse(kel_view0.begin(), kel_view0.end());
  // induction 2
  vector<int> kel_view1{40,41,42,43,44,45,46,47,48,49, 13,11,10,9,7,6,4,3,2,0};
  // collection
  vector<int> kel_view2{14,16,17,19,20,22,23,24,26,27,29,30,31,33,34,36,37,39};
  std::reverse(kel_view2.begin(), kel_view2.end());

  
  dune::tde::crp_connectors crp_conn( 0 ); //, nview );
  int ch_start = 0;
  for( auto const k : kel_view0 ){
    //crp_conn.add_connector( k, 0, true, ch_start );
    crp_conn.add_connector( k, 0, false, ch_start );
    ch_start += dune::tde::ch_per_kel;
  }
  ch_start = 0;
  for( auto const k : kel_view1 ){
    //bool reverse = (k <= 13);
    bool reverse = (k > 13);
    crp_conn.add_connector( k, 1, reverse, ch_start );
    ch_start += dune::tde::ch_per_kel;
  }

  ch_start = 0;
  for( auto const k : kel_view2 ){
    //crp_conn.add_connector( k, 2, true, ch_start );
    crp_conn.add_connector( k, 2, false, ch_start );
    ch_start += dune::tde::ch_per_kel;
  }

  // only one crp
  unsigned crp_id  = 0;
  unsigned seqn    = 0;

  // map the existing DAQ channels to 4th view
  // not connected view id
  unsigned view_na    = (unsigned)nview;
  unsigned view_na_ch = 0;

  for( auto const &utca : crates ) {
    unsigned utca_id = (unsigned)utca._id;
    //if( utca_id <= 1 ) continue;
    auto utca_conn   = utca._crp_conn;
    auto utca_nconn  = utca_conn.size();
    auto utca_slots  = (unsigned)utca._cards;
    for( unsigned amc = 0; amc < utca_slots; ++amc ){
      // unconnected AMCs
      if( amc >= utca_nconn ){
	for( unsigned cardch = 0; cardch < dune::tde::ch_per_amc; ++cardch ){
	  add( seqn++, utca_id, amc, cardch, crp_id, view_na, view_na_ch++, 1);
	}
	continue;
      }
      
      // connected to CRU
      auto conn = utca_conn[ amc ];
      unsigned islot = (unsigned)std::get<0>(conn);
      //unsigned icrp  = (unsigned)std::get<1>(conn);
      auto kel1_id  = std::get<2>(conn);
      auto kel2_id  = std::get<3>(conn);
      if( islot != amc ){
	throw cet::exception("VDColdboxTDEChannelMap")
	  <<"Mismatch in slot and AMC index: slot "
	  << islot<<" != amc "<<amc<<"\n";
	//amc = islot;
      }

      // 1st connector
      if( kel1_id < (int)crp_conn._kels.size() ){
	auto kel_ = std::next(crp_conn._kels.begin(), kel1_id);
	if( kel_->_id != kel1_id ) {
	  throw cet::exception("VDColdboxTDEChannelMap")
	    <<"Mismatch in KEL1 numbering\n";
	}
	
	vector<unsigned> order_ = (kel_->_reverse)? kel_inv : kel_nor;
	unsigned iview     = (unsigned)kel_->_view;
	unsigned vch_start = (unsigned)kel_->_first_view_ch;
	for( unsigned cardch = 0; cardch < dune::tde::ch_per_kel; ++cardch ){
	  unsigned viewch = vch_start + order_[cardch];
	  add( seqn++, utca_id, amc, cardch, crp_id, iview, viewch, 0);
	}
      } // 1st connector
      
      // 2nd connector
      if( kel2_id < (int)crp_conn._kels.size() ){
	auto kel_ = std::next(crp_conn._kels.begin(), kel2_id);
	if( kel_->_id != kel2_id ) {
	  throw cet::exception("VDColdboxTDEChannelMap")
	    <<"Mismatch in KEL2 numbering\n";
	}
	
	vector<unsigned> order_ = (kel_->_reverse)? kel_inv : kel_nor;
	unsigned iview     = (unsigned)kel_->_view;
	unsigned vch_start = (unsigned)kel_->_first_view_ch;
	for( unsigned cardch = 0; cardch < dune::tde::ch_per_kel; ++cardch ){
	  unsigned viewch = vch_start + order_[cardch];
	  add( seqn++, utca_id, amc, cardch + dune::tde::ch_per_kel, crp_id, iview, viewch, 0);
	}
      }// 2nd connector
    } // loop over AMCs
  } // loop over crates

  //cout<<" seqn = "<<seqn<<endl;
  // that is it...
}


// VD coldbox CRP2 channel map
void dune::VDColdboxTDEChannelMapService::vdcb2crpMap(){
  int nslots  = 10;
  int nview   = 3;

  // utca crate 1
  tde::crate c1( 0, nslots );
  vector<int> c1_kel{4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
  c1.add_crp_connection( 0, 0, c1_kel );

  // utca crate 2
  tde::crate c2( 1, nslots );
  vector<int> c2_kel{24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43};
  c2.add_crp_connection( 0, 0, c2_kel );

  // utca crate 3
  tde::crate c3( 2, nslots );
  vector<int> c3_kel{44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63};
  c3.add_crp_connection( 0, 0, c3_kel );

  // utca crate 4
  tde::crate c4( 3, nslots );
  vector<int> c4_kel{0,1,2,3,94,95,92,93,90,91,88,89,86,87,84,85,82,83};
  c4.add_crp_connection( 0, 0, c4_kel );

  // utca crate 5
  tde::crate c5( 4, nslots );
  vector<int> c5_kel{80,81,78,79,76,77,74,75,72,73,70,71,68,69,66,67,64,65};
  c5.add_crp_connection( 0, 0, c5_kel );


  
  vector<tde::crate> crates{c1, c2, c3, c4, c5};

  //
  // channel order with 8 ch inversion and flange swap
  vector<unsigned> kel_order = {24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22,
				23,  8,  9, 10, 11, 12, 13, 14, 15,  0,  1,  2,  3,  4,  5,  6,  7 };

  
  // kel connectors for each view sorted in the view channel order
  // induction 1
  vector<int> kel_view0{12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,37,41,45,
			93,89,85,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60};
  // induction 2
  vector<int> kel_view1{1,5,9,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,
			83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60,57,53,49};

  // collection
  vector<int> kel_view2{12,11,10, 8, 7, 6, 4, 3, 2, 0, // reversed
			35,36,38,39,40,42,43,44,46,47, // not reversed
  			95,94,92,91,90,88,87,86,84,83, // reveresed
  			48,50,51,52,54,55,56,58,59,60  // not reversed
  };

  // pin mappings
  tde::crp_connectors crp_conn( 0 );
  short nkel_cru = dune::tde::vdcb_crp_v1::nkel_cru;
  auto pin_map   = dune::tde::vdcb_crp_v1::pin_map;
  int ch_start = 0;

  //ind1
  for( auto const k : kel_view0 ){
    bool reverse = (k >= nkel_cru );
    auto pm      = pin_map[k%nkel_cru];
    short nch    = dune::tde::viewChCount( pm, dune::tde::Ind1 );
    crp_conn.add_connector( k, dune::tde::Ind1, reverse, ch_start, nch );
    ch_start += nch;
  }
  
  // ind2
  ch_start = 0;
  for( auto const k : kel_view1 ){
    bool reverse = (k >= nkel_cru );
    auto pm   = pin_map[k%nkel_cru];
    short nch = dune::tde::viewChCount( pm, dune::tde::Ind2 );
    crp_conn.add_connector( k, dune::tde::Ind2, reverse, ch_start, nch );
    ch_start += nch;
  }
  
  // col 
  ch_start = 0;
  for( auto const k : kel_view2 ){
    bool reverse = false;
    if( (k <= 12) ||
	(k >= 83 && k <= 95) ){
	reverse = true;
    }
    auto pm   = pin_map[k%nkel_cru];
    short nch = dune::tde::viewChCount( pm, dune::tde::Col );
    crp_conn.add_connector( k, dune::tde::Col, reverse, ch_start, nch );
    ch_start += nch;
  }

  /*
  size_t ch_tot = 0;
  for( auto const &k : crp_conn._kels ){
    k.print();
    ch_tot += k._nch;
  }
  cout<<"channels in total "<<ch_tot<<endl;
  //auto conn_views = crp_conn.get_connector_views( 12 );
  */
  unsigned errval  = std::numeric_limits<unsigned>::max(); 
  
  // only one crp
  unsigned crp_id  = 0;
  unsigned seqn    = 0;

  unsigned view_na    = (unsigned)nview;
  unsigned view_na_ch = 0;
  
  for( auto const &utca : crates ) {
    unsigned utca_id = (unsigned)utca._id;
    //if( utca_id <= 1 ) continue;
    auto utca_conn   = utca._crp_conn;
    auto utca_nconn  = utca_conn.size();
    auto utca_slots  = (unsigned)utca._cards; // max possible cards per crate
    for( unsigned amc = 0; amc < utca_slots; ++amc ){
      // unconnected AMCs
      if( amc >= utca_nconn ){
	for( unsigned cardch = 0; cardch < dune::tde::ch_per_amc; ++cardch ){
	  seqn = (utca_id * utca_slots + amc ) * dune::tde::ch_per_amc + cardch % dune::tde::ch_per_amc;
	  add( seqn, utca_id, amc, cardch, crp_id, view_na, view_na_ch++, 1);
	}
	continue;
      }
      
      // connected to CRU
      auto conn = utca_conn[ amc ];
      unsigned islot   = (unsigned)std::get<0>(conn);
      //unsigned icrp  = (unsigned)std::get<1>(conn);
      auto kel1_id  = std::get<2>(conn);
      auto kel2_id  = std::get<3>(conn);
      if( islot != amc ){
	throw cet::exception("VDColdboxTDEChannelMap")
	  <<"Mismatch in slot and AMC index. Should not happen\n";
	amc = islot;
      }
      
      { // 1st connector
	auto conn_views = crp_conn.get_connector_views( kel1_id );
	if( conn_views.empty() ){
	  throw cet::exception("VDColdboxTDEChannelMap")
	    <<"Could not find KEL "<<kel1_id<<"\n"; continue;
	}

	auto pm = pin_map[kel1_id % nkel_cru];
	for( const auto &kel : conn_views ){
	  vector<unsigned> order_ = kel_order; ////(kel._reverse)? kel_inv : kel_nor;
	  int iview   = kel._view;
	  int viewch  = kel._first_view_ch;
	  int vwchstp = 1;
	  if( kel._reverse ){
	    viewch += kel._nch - 1;
	    vwchstp = -1;
	  }
	  //cout<<" amc "<<amc<<"    KEL connectors -> "<<kel1_id<<"* "
	  //<<kel2_id<<" "<<iview<<" "<<viewch<<endl;    
	  for( unsigned kelch = 0; kelch < dune::tde::ch_per_kel; ++kelch ){
	    if( pm[kelch] != iview ) continue;
	    unsigned cardch = order_[ kelch ];
	    assert( cardch != errval ); //all channels should be unique
	    order_[kelch] = errval;     //mark this channel as used
	    seqn = (utca_id * utca_slots + amc) * dune::tde::ch_per_amc + cardch % dune::tde::ch_per_amc;
	    add( seqn, utca_id, amc, cardch, crp_id,
		 (unsigned)iview, (unsigned)viewch, 0 );
	    viewch += vwchstp;
	  }
	}
      }// 1st connector

      {// 2nd connector
	auto conn_views = crp_conn.get_connector_views( kel2_id );
	if( conn_views.empty() ){
	  throw cet::exception("VDColdboxTDEChannelMap")
	    <<"Could not find KEL "<<kel2_id<<"\n"; continue;
	}
	auto pm = pin_map[kel2_id % nkel_cru];
	for( const auto &kel : conn_views ){
	  vector<unsigned> order_ = kel_order; ////(kel._reverse)? kel_inv : kel_nor;
	  int iview  = kel._view;
	  int viewch = kel._first_view_ch;
	  int vwchstp = 1;
	  if( kel._reverse ){
	    viewch += kel._nch - 1;
	    vwchstp = -1;
	  }
	  //cout<<" amc "<<amc<<"    KEL connectors -> "<<kel1_id<<" "
	  //<<kel2_id<<"* "<<iview<<" "<<viewch<<endl;
	  for( unsigned kelch = 0; kelch < dune::tde::ch_per_kel; ++kelch ){
	    if( pm[kelch] != iview ) continue;
	    unsigned cardch = order_[ kelch ];
	    assert( cardch != errval ); //all channels should be unique
	    order_[kelch] = errval;     //mark this channel as used
	    cardch += dune::tde::ch_per_kel;  // add offset for previous connector
	    seqn = (utca_id * utca_slots + amc) * dune::tde::ch_per_amc + cardch % dune::tde::ch_per_amc;
	    add( seqn, utca_id, amc, cardch, crp_id,
		 (unsigned)iview, (unsigned)viewch, 0 );
	    viewch += vwchstp;
	  }
	}
      } // 2nd connector
    } // loop over AMCs
  } // loop over crates

  // that is it...
  //cout<<seqn<<endl;
}

DEFINE_ART_SERVICE(dune::VDColdboxTDEChannelMapService)
