//File: CRTVDTrigger.h
//
// T Kosc : based on CRT::Trigger
// kosc.thomas@gmail.com
//Brief: A CRTVD::Trigger stores the activity in the pair of ProtoDUNE-VD CRT modules when the modules triggered themselves to be read out.  
//       CRT modules trigger a readout when there is time correlated activity in both CRT modules, labeled TOP and BOTTOM
//       in reference to their position on the cryostat.
//       When there is a trigger, all channels'activity is read out, regardless of ADC counts (threshold regardless).  
//       A channel with above-baseline(?) activity at the time of readout is represented as a 
//       CRTVD::Hit stored by the owned CRTVD::Trigger.  So, a CRTVD::Trigger can only have 0 CRTVD::Hits if it was default-constructed.  
//

//Include guards so that this file doesn't interfere with itself when included multiple times
#ifndef CRTVD_TRIGGER_H
#define CRTVD_TRIGGER_H

#include "larcoreobj/SimpleTypesAndConstants/geo_vectors.h"
#include "larcorealg/GeoAlgo/GeoVector.h"

//c++ includes
#include <cstdint>
#include <vector>
#include <limits>
//#include <TVector3.h>

namespace CRTVD
{
  class Hit
  {
    public:
      //Constructor from information in CRT::Fragment plus detector name for LArSoft
      Hit(uint8_t channel, const std::string & detName, float edep, geo::Point_t pos): fChannel(channel), fAuxDetName(detName), fEdep(edep), fPosition(pos)
      {
      }


      //Default constructor to make ROOT happy.  Set ADC peak to -1 and everything else to largest possible value so that default-constructed 
      //hits are obvious.  
      Hit(): fChannel(std::numeric_limits<decltype(fChannel)>::max()), fAuxDetName("NULL"), fADC(std::numeric_limits<decltype(fADC)>::max()), fPosition(geo::Point_t()) {}

      //No resources managed by a CRT::Hit, so use default destructor
      virtual ~Hit() = default; 
  
      //Public access to stored information
      inline size_t Channel() const { return fChannel; }

      inline std::string AuxDetName() const { return fAuxDetName; }

//      inline short ADC() const { return fADC; }
      float Edep() const { return fEdep; }

      geo::Point_t Position() const { return fPosition; }

      //Check whether this Hit was default-constructed
      inline bool IsDefault() const { return fADC == std::numeric_limits<decltype(fADC)>::max(); }

      //Print out to some ostream-like object for debugging
      template <class STREAM>
      void dump(STREAM& stream) const
      {
        stream << "CRT::Hit Dump:\n"
               << "Channel: " << fChannel << "\n"
               << "AuxDetName: " << fAuxDetName << "\n"
               << "ADC: " << fADC << "\n"
               << "Was this CRT::Hit default-constructed? " << (IsDefault()?"true":"false") << "\n";
//        return stream;
        return;
      }

    private:   
      //Information for identifying which CRT module strip this hit was recorded on 
      size_t fChannel; //The index of the AuxDetSensitiveGeo that represents this strip in a CRT module
      std::string fAuxDetName; //The name of the volume from the geometry that represents the CRT module strip this hit was recorded in. 
      short fADC; //Baseline-subtracted Analog to Digital Converter value at time when this hit was read out.  
      float fEdep; // (MeV)
      geo::Point_t fPosition; // Hit 3D position in cm
  }; // end class




  //Also overload operator << for std::ostream to make debugging CRT::Hits as easy as possible.  Really just call dump() internally.
  template <class STREAM>
  STREAM& operator << (STREAM& lhs, const CRTVD::Hit& hit)
  {
    return hit.dump(lhs);
  }





  class Trigger
  {
    public:
      //Constructor from information that comes from Matt's artdaq::Fragments.  Takes ownership of the CRT::Hits given.  Note that this 
      //is the only time that CRT::Hits can be added to a Trigger in accordance with some data product design notes I found.   
      //TODO: Should timestamp assembly be handled here or elsewhere?
      Trigger(const unsigned short trigtype, const unsigned long long timestamp, 
              std::vector<CRTVD::Hit>&& hits): fType(trigtype), fTimestamp(timestamp), fHits(hits) 
         {
         CalculateHitsMeanPositions();

         }

      Trigger(): fType(std::numeric_limits<decltype(fType)>::max()),  
                 fTimestamp(std::numeric_limits<decltype(fTimestamp)>::max()), fHits() {} //Default constructor to satisfy ROOT.  


      // Calculate hits mean positions
      void CalculateHitsMeanPositions(){
        // dumb init
        fTopMeanPosition = geoalgo::Point_t(0., 0., 0.);
        fBottomMeanPosition = geoalgo::Point_t(0., 0., 0.);
        int ntophit = 0; int nbothit = 0;

         for (auto hit : fHits){
           geoalgo::Point_t pos(hit.Position().X(), hit.Position().Y(), hit.Position().Z());
           if (hit.AuxDetName().find("BOTTOM")==hit.AuxDetName().npos){
             ntophit++;
             fTopMeanPosition += pos;
           }
           else{
             nbothit++;
             fBottomMeanPosition += pos;
           }
         } // end for

      if (ntophit>0) fTopMeanPosition /= (float) ntophit;
      if (nbothit>0) fBottomMeanPosition /= (float) nbothit;
//std::cout  << "CRTVD::Trigger : top and bottom mean hit positions are : \n";
//std::cout << "TOP : "  << fTopMeanPosition[0] << " " << fTopMeanPosition[1] << " " << fTopMeanPosition[2] << std::endl;
//std::cout << "BOTTOM : "  << fBottomMeanPosition[0] << " " << fBottomMeanPosition[1] << " " << fBottomMeanPosition[2] << std::endl;

      } // end InitHitsMeanPositions()
        

      //User access to stored information.  See member variables for explanation
      inline unsigned short TriggerType() const { return fType; }
      inline unsigned long long Timestamp() const { return fTimestamp; }
      inline const std::vector<CRTVD::Hit>& Hits() const { return fHits; }

      //Check whether this Hit was default-constructed
      inline bool IsDefault() const { return fType == std::numeric_limits<decltype(fType)>::max(); }

      //Dumping an object is also helpful for debugging.  
      template <class STREAM>
      void dump(STREAM& stream) const
      {
        stream << "CRTVD::Trigger dump:\n"
               << "Trigger Type: " << fType << "\n"
//               << "Detector Name: " << fDetName << "\n"
               << "Timestamp: " << fTimestamp << "\n"
               << "Hits:\n";
        for(const auto& hit: fHits) hit.dump(stream);

        return stream;
      }

    private:
      unsigned short fType; // trigger type. 0 : TOP crt module only, BOTTOM only, 2 : coincidence

      unsigned long long fTimestamp; //Timestamp when this Trigger occurred.  First 32 bits are Linux time since epoch; last 32 bits are time in 
                                     //ns. 
      std::vector<CRTVD::Hit> fHits; //All activity in CRT strips within this module when it was read out 
      geoalgo::Point_t fTopMeanPosition;
      geoalgo::Point_t fBottomMeanPosition;
  };

  //ostream operator overload so users can do mf::LogWarning("CRTTriggerError") << /*stuff*/ << trigger << "\n". 
  template <class STREAM>
  STREAM& operator <<(STREAM& lhs, const CRTVD::Trigger& trigger)
  {
    return trigger.dump(lhs);
  }
}

#endif //CRTVD_TRIGGER_H
