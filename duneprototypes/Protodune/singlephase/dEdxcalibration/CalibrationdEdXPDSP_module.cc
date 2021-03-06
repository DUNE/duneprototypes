////////////////////////////////////////////////////////////////////////
// Class:       CalibrationdEdXPDSP
// Module Type: producer
// File:        CalibrationdEdXPDSP_module.cc
//
// Generated at Thu Nov 30 15:55:16 2017 by Tingjun Yang using artmod
// from cetpkgsupport v1_13_00.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "canvas/Utilities/InputTag.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "lardata/Utilities/AssociationUtil.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "larreco/Calorimetry/CalorimetryAlg.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/AnalysisBase/Calorimetry.h"
#include "dunecalib/Calib/XYZCalib.h"
#include "dunecalib/CalibServices/XYZCalibService.h"
#include "dunecalib/Calib/LifetimeCalib.h"
#include "dunecalib/CalibServices/LifetimeCalibService.h"

#include "larevt/SpaceCharge/SpaceCharge.h"
#include "larevt/SpaceChargeServices/SpaceChargeService.h"

#include "TH2F.h"
#include "TH1F.h"
#include "TFile.h"
#include "TTimeStamp.h"

#include <memory>

namespace dune {
  class CalibrationdEdXPDSP;
}

class dune::CalibrationdEdXPDSP : public art::EDProducer {
public:
  explicit CalibrationdEdXPDSP(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  CalibrationdEdXPDSP(CalibrationdEdXPDSP const &) = delete;
  CalibrationdEdXPDSP(CalibrationdEdXPDSP &&) = delete;
  CalibrationdEdXPDSP & operator = (CalibrationdEdXPDSP const &) = delete;
  CalibrationdEdXPDSP & operator = (CalibrationdEdXPDSP &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

  // Selected optional functions.
  void beginJob() override;

private:

  std::string fTrackModuleLabel, fShowerModuleLabel;
  std::string fCalorimetryModuleLabel, fShowerCalorimetryModuleLabel;
  std::string fHitModuleLabel;

  calo::CalorimetryAlg caloAlg;

  double fModBoxA;
  double fModBoxB;

  bool fSCE;
  bool fApplyNormCorrection, fApplyNormCorrectionShower;
  bool fApplyXCorrection, fApplyXCorrectionShower;
  bool fApplyYZCorrection, fApplyYZCorrectionShower;
  bool fApplyLifetimeCorrection, fApplyLifetimeCorrectionShower;
  bool fCorrectResidualRange;
  double fShowerRecombFactor;
  bool fUseLifetimeFromDatabase; // true: lifetime from database; false: lifetime from DetectorProperties
  std::vector<double> fReferencedQdx; 

  double fLifetime; // [us]

  double vDrift;
  double xAnode;

  bool first;

  TH1D *hdRR[3];
  
  void CorrectResidualRange(double endx, double endy, double endz, std::vector<float> &vresRange, std::vector<geo::Point_t> vXYZ, int plane);
};


dune::CalibrationdEdXPDSP::CalibrationdEdXPDSP(fhicl::ParameterSet const & p)
  : EDProducer(p)
  , fTrackModuleLabel      (p.get< std::string >("TrackModuleLabel"))
  , fShowerModuleLabel      (p.get< std::string >("ShowerModuleLabel"))
  , fCalorimetryModuleLabel(p.get< std::string >("CalorimetryModuleLabel"))
  , fShowerCalorimetryModuleLabel(
      p.get< std::string >("ShowerCalorimetryModuleLabel"))
  , fHitModuleLabel        (p.get< std::string >("HitModuleLabel"))
  , caloAlg                (p.get< fhicl::ParameterSet >("CaloAlg"))
  , fModBoxA               (p.get< double >("ModBoxA"))
  , fModBoxB               (p.get< double >("ModBoxB"))
  , fSCE                   (p.get< bool >("CorrectSCE"))
  , fApplyNormCorrection   (p.get< bool >("ApplyNormCorrection"))
  , fApplyNormCorrectionShower   (p.get< bool >("ApplyNormCorrectionShower"))
  , fApplyXCorrection      (p.get< bool >("ApplyXCorrection"))
  , fApplyXCorrectionShower      (p.get< bool >("ApplyXCorrectionShower"))
  , fApplyYZCorrection     (p.get< bool >("ApplyYZCorrection"))
  , fApplyYZCorrectionShower     (p.get< bool >("ApplyYZCorrectionShower"))
  , fApplyLifetimeCorrection(p.get< bool >("ApplyLifetimeCorrection"))
  , fApplyLifetimeCorrectionShower(p.get< bool >("ApplyLifetimeCorrectionShower"))
  , fCorrectResidualRange(p.get<bool>("CorrectResidualRange", false))
  , fShowerRecombFactor(p.get<double>("ShowerRecombFactor", 1.))
  , fUseLifetimeFromDatabase(p.get< bool >("UseLifetimeFromDatabase"))
  , fReferencedQdx         (p.get<std::vector<double>>("ReferencedQdx"))
{
  auto const clockData = art::ServiceHandle<detinfo::DetectorClocksService>()->DataForJob();
  auto const detProp = art::ServiceHandle<detinfo::DetectorPropertiesService>()->DataForJob(clockData);
  vDrift = detProp.DriftVelocity(); //cm/us
  xAnode = std::abs(detProp.ConvertTicksToX(trigger_offset(clockData),0,0,0));
  //create calorimetry product and its association with track
  produces< std::vector<anab::Calorimetry>              >();
  produces< art::Assns<recob::Track, anab::Calorimetry> >();
  produces< art::Assns<recob::Shower, anab::Calorimetry> >();
  first = true;
}

void dune::CalibrationdEdXPDSP::produce(art::Event & evt)
{

  art::ServiceHandle<calib::XYZCalibService> xyzcalibHandler;
  calib::XYZCalibService & xyzcalibService = *xyzcalibHandler;
  calib::XYZCalib *xyzcalib = xyzcalibService.provider();

  // Electron lifetime from database calibration service provider
  art::ServiceHandle<calib::LifetimeCalibService> lifetimecalibHandler;
  calib::LifetimeCalibService & lifetimecalibService = *lifetimecalibHandler;
  calib::LifetimeCalib *lifetimecalib = lifetimecalibService.provider();
  auto const detProp = art::ServiceHandle<detinfo::DetectorPropertiesService const>()->DataFor(evt);

  if (fApplyLifetimeCorrection || fApplyLifetimeCorrectionShower) {
    if (fUseLifetimeFromDatabase) {
      fLifetime = lifetimecalib->GetLifetime()*1000.0; // [ms]*1000.0 -> [us]
      //std::cout << "use lifetime from database   " << fLifetime << std::endl;
    }
    else {
      fLifetime = detProp.ElectronLifetime(); // [us]
    }
  }

  //int run = evt.run();
  //int subrun = evt.subRun();
  //int event = evt.id().event();
  // evttime
  art::Timestamp ts = evt.time();
  TTimeStamp tts(ts.timeHigh(), ts.timeLow());
  uint64_t evttime = tts.AsDouble();

  std::cout << "run: " << evt.run() << " ; subrun: " << evt.subRun() << " ; event: " << evt.id().event() << std::endl;
  std::cout << "evttime: " << evttime << std::endl;
  if (fApplyLifetimeCorrection) std::cout << "fLifetime: " << fLifetime << " [us]" << std::endl;
  if (first){
    std::cout<<"fApplyNormCorrection: "<<fApplyNormCorrection<<std::endl;
    std::cout<<"fApplyXCorrection: "<<fApplyXCorrection<<std::endl;
    std::cout<<"fApplyYZCorrection: "<<fApplyYZCorrection<<std::endl;
    std::cout<<"fApplyLifetimeCorrection: "<<fApplyLifetimeCorrection<<std::endl;
    std::cout<<"fUseLifetimeFromDatabase: "<<fUseLifetimeFromDatabase<<std::endl;
    std::cout<<"Plane\tReference dQ/dx\tGlobal median dQ/dx\tCalorimetry constant"<<std::endl;
    for (int i = 0; i<3; ++i){
      printf("Plane[%d]\t%f\t%f\t%f\n",i,fReferencedQdx[i], xyzcalib->GetNormCorr(i), 1./caloAlg.ElectronsFromADCArea(1, i));
    }
    first = false;
  }
  //Spacecharge services provider
  auto const* sce = lar::providerFrom<spacecharge::SpaceChargeService>();

  //create anab::Calorimetry objects and make association with recob::Track
  std::unique_ptr< std::vector<anab::Calorimetry> > calorimetrycol(new std::vector<anab::Calorimetry>);
  std::unique_ptr< art::Assns<recob::Track, anab::Calorimetry> > assn(new art::Assns<recob::Track, anab::Calorimetry>);

  //get existing track/calorimetry objects
  auto trackListHandle = evt.getHandle< std::vector<recob::Track> >(fTrackModuleLabel);

  std::vector<art::Ptr<recob::Track>> tracklist;
  art::fill_ptr_vector(tracklist, trackListHandle);

  art::FindManyP<anab::Calorimetry> fmcal(trackListHandle, evt, fCalorimetryModuleLabel);

  //get hits
  auto hitListHandle = evt.getHandle< std::vector<recob::Hit> >(fHitModuleLabel);

  std::vector<art::Ptr<recob::Hit>> hitlist;
  art::fill_ptr_vector(hitlist, hitListHandle);

  if (!fmcal.isValid()){
    throw art::Exception(art::errors::ProductNotFound)
      <<"Could not get assocated Calorimetry objects";
  }

  for (size_t trkIter = 0; trkIter < tracklist.size(); ++trkIter){
    
    for (size_t i = 0; i<fmcal.at(trkIter).size(); ++i){
      auto & calo = fmcal.at(trkIter)[i];

      if (!(calo->dEdx()).size()){
        //empty calorimetry product, just copy it
        calorimetrycol->push_back(*calo);
        util::CreateAssn(*this, evt, *calorimetrycol, tracklist[trkIter], *assn);
      }
      else{
        //start calibrating dQdx

        //get original calorimetry information
        //double                Kin_En     = calo->KineticEnergy();
        std::vector<float>   vdEdx      = calo->dEdx();
        std::vector<float>   vdQdx      = calo->dQdx();
        std::vector<float>   vresRange  = calo->ResidualRange();
        std::vector<float>   deadwire   = calo->DeadWireResRC();
        float                Trk_Length = calo->Range();
        std::vector<float>   fpitch     = calo->TrkPitchVec();
        const auto&          fHitIndex  = calo->TpIndices();
        const auto&          vXYZ       = calo->XYZ();
        geo::PlaneID         planeID    = calo->PlaneID();

        //make sure the vectors are of the same size
        if (vdEdx.size()!=vXYZ.size()||
            vdQdx.size()!=vXYZ.size()||
            vresRange.size()!=vXYZ.size()||
            fpitch.size()!=vXYZ.size()){
          throw art::Exception(art::errors::Configuration)
      <<"Vector sizes mismatch for vdEdx, vdQdx, vresRange, fpitch, vXYZ";
        }

        //make sure the planeID is reasonable
        if (!planeID.isValid){
          throw art::Exception(art::errors::Configuration)
      <<"planeID is invalid";
        }
        if (planeID.Plane>2){
          throw art::Exception(art::errors::Configuration)
            <<"plane is invalid "<<planeID.Plane;
        }
        // update the kinetic energy
        double EkinNew = 0.;

        for (size_t j = 0; j<vdQdx.size(); ++j){
          auto & hit = hitlist[fHitIndex[j]];
          if (hit->WireID().Plane != planeID.Plane){
            throw art::Exception(art::errors::Configuration)
              <<"Hit plane = "<<hit->WireID().Plane<<" calo plane = "<<planeID.Plane;
          }
          double normcorrection = 1;
          if (fApplyNormCorrection){
            normcorrection = xyzcalib->GetNormCorr(planeID.Plane);
            if (normcorrection) normcorrection = fReferencedQdx[planeID.Plane]/normcorrection;
            if (!normcorrection) normcorrection = 1.;
          }
          double xcorrection = 1;
          if (fApplyXCorrection){
            xcorrection = xyzcalib->GetXCorr(planeID.Plane, vXYZ[j].X());
            if (!xcorrection) xcorrection = 1.;
          }
          double yzcorrection = 1;
          if (fApplyYZCorrection){
            yzcorrection = xyzcalib->GetYZCorr(planeID.Plane, vXYZ[j].X()>0, vXYZ[j].Y(), vXYZ[j].Z());
            if (!yzcorrection) yzcorrection = 1.;
          }
          if (fApplyLifetimeCorrection){
            xcorrection *= exp((xAnode-std::abs(vXYZ[j].X()))/(fLifetime*vDrift));
          }
          //if (planeID.Plane == 2 && tracklist[trkIter]->ID() == 0) std::cout<<"plane = "<<planeID.Plane<<" x = "<<vXYZ[j].X()<<" y = "<<vXYZ[j].Y()<<" z = "<<vXYZ[j].Z()<<" normcorrection = "<<normcorrection<<" xcorrection = "<<xcorrection<<" yzcorrection = "<<yzcorrection<<" "<<vdQdx[j]<<std::endl;

          vdQdx[j] = normcorrection*xcorrection*yzcorrection*vdQdx[j];


          //set time to be trgger time so we don't do lifetime correction
          //we will turn off lifetime correction in caloAlg, this is just to be double sure
          //vdEdx[j] = caloAlg.dEdx_AREA(vdQdx[j], detProp.TriggerOffset(), planeID.Plane, 0);


          //Calculate dE/dx uisng the new recombination constants
          double dQdx_e = caloAlg.ElectronsFromADCArea(vdQdx[j], planeID.Plane);
          double rho = detProp.Density();                       // LAr density in g/cm^3
          double Wion = 1000./util::kGeVToElectrons;    // 23.6 eV = 1e, Wion in MeV/e
          double E_field_nominal = detProp.Efield();   // Electric Field in the drift region in KV/cm

          //correct Efield for SCE
          geo::Vector_t E_field_offsets = {0., 0., 0.};

          if(sce->EnableCalEfieldSCE()&&fSCE) E_field_offsets = sce->GetCalEfieldOffsets(geo::Point_t{vXYZ[j].X(), vXYZ[j].Y(), vXYZ[j].Z()},hit->WireID().TPC);

          TVector3 E_field_vector = {E_field_nominal*(1 + E_field_offsets.X()), E_field_nominal*E_field_offsets.Y(), E_field_nominal*E_field_offsets.Z()};
          double E_field = E_field_vector.Mag();

          //calculate recombination factors
          double Beta = fModBoxB / (rho * E_field);
          double Alpha = fModBoxA;
          //double old_vdEdx = vdEdx[j];
          vdEdx[j] = (exp(Beta * Wion * dQdx_e) - Alpha) / Beta;
          //if (planeID.Plane == 2 && tracklist[trkIter]->ID() == 1) std::cout<<calo->dQdx()[j]<<" "<<vdEdx[j]<<" "<<vdQdx[j]<<" "<<vresRange[j]<<" "<<E_field<<" "<<rho<<" "<<Beta<<" "<<Alpha<<" "<<E_field_nominal<<" "<<E_field<<" "<<E_field_offsets.X()<<" "<<E_field_offsets.Y()<<" "<<E_field_offsets.Z()<<" "<<vXYZ[j].X()<<" "<<vXYZ[j].Y()<<" "<<vXYZ[j].Z()<<" "<<hit->WireID().TPC<<" "<<fHitIndex[j]<<" "<<normcorrection<<" "<<xcorrection<<" "<<yzcorrection<<std::endl;

         /*if (planeID.Plane==2){
         std::cout << sce->EnableCalEfieldSCE() << " " << fSCE << std::endl;
         std::cout << E_field << " " << E_field_nominal << std::endl;
         std::cout << vdQdx[j] << " " << dQdx_e << std::endl;
         std::cout << old_vdEdx << " " << vdEdx[j] << std::endl;
         std::cout << rho << " " << Wion << " " << Beta << "\n" << std::endl; }
         */
          //update kinetic energy calculation
          if (j>=1) {
            if ( (vresRange[j] < 0) || (vresRange[j-1] < 0) ) continue;
            EkinNew += fabs(vresRange[j]-vresRange[j-1]) * vdEdx[j];
          }
          if (j==0){
            if ( (vresRange[j] < 0) || (vresRange[j+1] < 0) ) continue;
            EkinNew += fabs(vresRange[j]-vresRange[j+1]) * vdEdx[j];
          }

        }

        if (fCorrectResidualRange && (!vresRange.empty())){
          double endx =tracklist[trkIter]->End().X();
          double endy =tracklist[trkIter]->End().Y();
          double endz =tracklist[trkIter]->End().Z();
          // find hit closest to the track end
          auto & hit = hitlist[fHitIndex[0]];
          if (vresRange[0]>vresRange.back()){
            hit = hitlist[fHitIndex.back()];
          }
          geo::Vector_t posOffsets = {0., 0., 0.};
          if (sce->EnableCalSpatialSCE() && fSCE){
            posOffsets = sce->GetCalPosOffsets(geo::Point_t(endx, endy, endz), hit->WireID().TPC);
            endx -= posOffsets.X();
            endy += posOffsets.Y();
            endz += posOffsets.Z();
          }
          CorrectResidualRange(endx, endy, endz, vresRange, vXYZ, hit->WireID().Plane);
        }
        
        //save new calorimetry information
        calorimetrycol->push_back(anab::Calorimetry(EkinNew,// Kin_En, // change by David C. to update kinetic energy calculation
                                                    vdEdx,
                                                    vdQdx,
                                                    vresRange,
                                                    deadwire,
                                                    Trk_Length,
                                                    fpitch,
                                                    vXYZ,
                                                    fHitIndex,
                                                    planeID));
        util::CreateAssn(*this, evt, *calorimetrycol, tracklist[trkIter], *assn);
      }//calorimetry object not empty
    }//loop over calorimetry objects
  }//loop over tracks


  auto showerListHandle = evt.getHandle<std::vector<recob::Shower>>(fShowerModuleLabel);
  std::vector<art::Ptr<recob::Shower> > showerlist;
  art::fill_ptr_vector(showerlist, showerListHandle);

  //CalibrateShowers(evt, hitlist, showerlist, shower_calos);

  //create anab::Calorimetry objects and make association with recob::Shower
  std::unique_ptr< art::Assns<recob::Shower, anab::Calorimetry>> assn_shower(
      new art::Assns<recob::Shower, anab::Calorimetry>);
  art::FindManyP<anab::Calorimetry> fmcal_shower(showerListHandle, evt,
                                                 fShowerCalorimetryModuleLabel);
  if (!fmcal_shower.isValid()){
    throw art::Exception(art::errors::ProductNotFound)
      << "Could not get assocated Calorimetry objects. "
      << "Shower Label: " << fShowerModuleLabel << " Shower Calo Label: "
      << fShowerCalorimetryModuleLabel << "\n";
  }

  for (size_t showerIt = 0; showerIt < showerlist.size(); ++showerIt) {
    for (size_t i = 0; i < fmcal_shower.at(showerIt).size(); ++i) {
      auto & calo = fmcal_shower.at(showerIt)[i];

      if (!calo->dEdx().size()) {
        //empty calorimetry product, just copy it
        calorimetrycol->push_back(*calo);
        util::CreateAssn(*this, evt, *calorimetrycol, showerlist[showerIt], *assn_shower);
      }
      else{
        //start calibrating dQdx

        //get original calorimetry information
        //double                Kin_En     = calo->KineticEnergy();
        std::vector<float>   vdEdx      = calo->dEdx();
        std::vector<float>   vdQdx      = calo->dQdx();
        std::vector<float>   vresRange  = calo->ResidualRange();
        std::vector<float>   deadwire   = calo->DeadWireResRC();
        float                length = calo->Range();
        std::vector<float>   fpitch     = calo->TrkPitchVec();
        const auto&          fHitIndex  = calo->TpIndices();
        const auto&          vXYZ       = calo->XYZ();
        geo::PlaneID         planeID    = calo->PlaneID();

        //make sure the vectors are of the same size
        if ((vdEdx.size() != vXYZ.size()) || (vdQdx.size() != vXYZ.size()) ||
            (vresRange.size() != vXYZ.size()) ||
            (fpitch.size() != vXYZ.size())) {
          throw art::Exception(art::errors::Configuration) << 
                "Vector sizes mismatch for vdEdx, vdQdx, vresRange, fpitch, vXYZ";
        }

        //make sure the planeID is reasonable
        if (!planeID.isValid) {
          throw art::Exception(art::errors::Configuration) << "planeID is invalid";
        }
        if (planeID.Plane>2){
          throw art::Exception(art::errors::Configuration) << "plane is invalid " <<
                planeID.Plane;
        }
        // update the kinetic energy
        double EkinNew = 0.;

        for (size_t j = 0; j < vdQdx.size(); ++j) {
          auto & hit = hitlist[fHitIndex[j]];
          if (hit->WireID().Plane != planeID.Plane) {
            throw art::Exception(art::errors::Configuration) <<"Hit plane = " <<
                  hit->WireID().Plane<<" calo plane = "<<planeID.Plane;
          }

          double normcorrection = 1;
          if (fApplyNormCorrectionShower) {
            normcorrection = xyzcalib->GetNormCorr(planeID.Plane);
            if (normcorrection) {
              normcorrection = fReferencedQdx[planeID.Plane]/normcorrection;
            }
            else {
              normcorrection = 1.;
            }
          }

          double xcorrection = 1;
          if (fApplyXCorrectionShower) {
            xcorrection = xyzcalib->GetXCorr(planeID.Plane, vXYZ[j].X());
            if (!xcorrection) xcorrection = 1.;
          }

          double yzcorrection = 1;
          if (fApplyYZCorrectionShower) {
            yzcorrection = xyzcalib->GetYZCorr(planeID.Plane, (vXYZ[j].X()>0),
                                               vXYZ[j].Y(), vXYZ[j].Z());
            if (!yzcorrection) yzcorrection = 1.;
          }

          if (fApplyLifetimeCorrectionShower){
            xcorrection *= exp((xAnode-std::abs(vXYZ[j].X()))/(fLifetime*vDrift));
          }

          vdQdx[j] = normcorrection*xcorrection*yzcorrection*vdQdx[j];


          //Calculate dE/dx uisng the new recombination constants
          double dQdx_e = caloAlg.ElectronsFromADCArea(vdQdx[j], planeID.Plane);
          double rho = detProp.Density();                       // LAr density in g/cm^3
          double Wion = 1000./util::kGeVToElectrons;    // 23.6 eV = 1e, Wion in MeV/e
          double E_field_nominal = detProp.Efield();   // Electric Field in the drift region in KV/cm

          //correct Efield for SCE
          geo::Vector_t E_field_offsets = {0., 0., 0.};

          if (sce->EnableCalEfieldSCE() && fSCE)
            E_field_offsets = sce->GetCalEfieldOffsets(
                geo::Point_t{vXYZ[j].X(), vXYZ[j].Y(), vXYZ[j].Z()},
                hit->WireID().TPC);

          TVector3 E_field_vector = {
            E_field_nominal*(1 + E_field_offsets.X()),
            E_field_nominal*E_field_offsets.Y(),
            E_field_nominal*E_field_offsets.Z()
          };

          double E_field = E_field_vector.Mag();

          //calculate recombination factors
          double Beta = fModBoxB / (rho * E_field);
          double Alpha = fModBoxA;
          //double old_vdEdx = vdEdx[j];
          vdEdx[j] = (exp(Beta * Wion * dQdx_e) - Alpha) / Beta;


          double hit_energy = hit->Integral();
          hit_energy *= normcorrection;
          hit_energy *= Wion/*23.6e-6*/;
          hit_energy *= caloAlg.ElectronsFromADCArea(1., planeID.Plane); //Returns 1./calib_factor
          hit_energy *= xcorrection;
          hit_energy *= yzcorrection;
          hit_energy /= fShowerRecombFactor;

          EkinNew += hit_energy;

        }
        //save new calorimetry information
        calorimetrycol->push_back(anab::Calorimetry(EkinNew,
                                                    vdEdx,
                                                    vdQdx,
                                                    vresRange,
                                                    deadwire,
                                                    length,
                                                    fpitch,
                                                    vXYZ,
                                                    fHitIndex,
                                                    planeID));
        util::CreateAssn(*this, evt, *calorimetrycol, showerlist[showerIt], *assn_shower);
      }//calorimetry object not empty
    }//loop over calorimetry objects
  }

  evt.put(std::move(calorimetrycol));
  evt.put(std::move(assn));
  evt.put(std::move(assn_shower));

  return;
}

void dune::CalibrationdEdXPDSP::beginJob(){
  art::ServiceHandle<art::TFileService const> tfs;
  for (int i = 0; i<3; ++i){
    hdRR[i] = tfs->make<TH1D>(Form("hdRR%d",i), Form("Plane %d;new RR - old RR (cm)",i), 100,-5,45);
    hdRR[i]->Sumw2();
  }
}
  
  

void dune::CalibrationdEdXPDSP::CorrectResidualRange(double endx, double endy, double endz, std::vector<float> &vresRange, std::vector<geo::Point_t> vXYZ, int plane){

  bool revDir = vresRange[0] > vresRange.back();
  for (size_t i = 0; i<vresRange.size(); ++i){
    if (!revDir){
      if (i==0){
        double newrr = sqrt(pow(vXYZ[i].X()-endx,2) + pow(vXYZ[i].Y()-endy,2) + pow(vXYZ[i].Z()-endz,2));
        hdRR[plane]->Fill(newrr-vresRange[i]);
        vresRange[i] = newrr;
      }
      else{
        vresRange[i] = vresRange[i-1] + sqrt(pow(vXYZ[i].X()-vXYZ[i-1].X(),2) +
                                             pow(vXYZ[i].Y()-vXYZ[i-1].Y(),2) +
                                             pow(vXYZ[i].Z()-vXYZ[i-1].Z(),2));
      }
    }
    else{
      int index = vresRange.size() - i -1;
      if (i==0){
        double newrr = sqrt(pow(vXYZ[index].X()-endx,2) + pow(vXYZ[index].Y()-endy,2) + pow(vXYZ[index].Z()-endz,2));
        hdRR[plane]->Fill(newrr-vresRange[index]);
        vresRange[index] = newrr;
      }
      else{
        vresRange[index] = vresRange[index+1] + sqrt(pow(vXYZ[index].X()-vXYZ[index+1].X(),2) +
                                                     pow(vXYZ[index].Y()-vXYZ[index+1].Y(),2) +
                                                     pow(vXYZ[index].Z()-vXYZ[index+1].Z(),2));
      }
    }
  }
}

DEFINE_ART_MODULE(dune::CalibrationdEdXPDSP)
