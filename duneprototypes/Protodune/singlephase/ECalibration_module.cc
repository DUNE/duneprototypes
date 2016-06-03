////////////////////////////////////////////////////////////////////////
// Class:       ECalibration
// Module Type: analyzer
// File:        ECalibration_module.cc
//
// Generated at Thu Mar 17 12:00:08 2016 by Dorota Stefan using artmod
// from cetpkgsupport v1_10_01.
//
// dorota.stefan@cern.ch
////////////////////////////////////////////////////////////////////////

#include "larsim/Simulation/LArG4Parameters.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"
#include "lardata/RecoBase/Hit.h"
#include "lardata/RecoBase/Cluster.h"
#include "lardata/RecoBase/Track.h"
#include "lardata/RecoBase/Vertex.h"
#include "lardata/RecoBase/Shower.h"
#include "lardata/AnalysisAlg/CalorimetryAlg.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "larcoreobj/SimpleTypesAndConstants/PhysicalConstants.h"
#include "lardata/Utilities/DatabaseUtil.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "TH1.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TVector3.h"

#include <cmath>

namespace proto
{
	class ECalibration;
}

class proto::ECalibration : public art::EDAnalyzer {
public:
  explicit ECalibration(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  ECalibration(ECalibration const &) = delete;
  ECalibration(ECalibration &&) = delete;
  ECalibration & operator = (ECalibration const &) = delete;
  ECalibration & operator = (ECalibration &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;

	void beginJob() override;

	void beginRun(const art::Run& run) override;
	
	void reconfigure(fhicl::ParameterSet const& p) override;
private:

  // Declare member data here.
	void ResetVars();

	double GetEkreco(double t0);

	double fEkinreco;

	geo::GeometryCore const * fGeometry;
	double fElectronsToGeV;

	int fRun;
	int fEvent;
	
	int fSimPdg;
	int fSimTrackID;

	bool fStopping;
	bool fDecaying;

	TH1D* fMomentumStartHist;
	TH1D* fMomentumEndHist;

	TTree *fTree;	

	std::vector< art::Ptr<simb::MCParticle> > fSimlist;
	std::vector< art::Ptr<recob::Hit> > fHitlist;
	std::vector< art::Ptr<recob::Track> > fTracklist;
	std::vector< art::Ptr<recob::Vertex> > fVertexlist;
	std::vector< art::Ptr<recob::Shower> > fShslist;

	// Module labels to get data products
	std::string fSimulationProducerLabel;
	std::string fHitsModuleLabel;
	std::string fClusterModuleLabel;
	std::string fTrackModuleLabel;	
	std::string fShowerModuleLabel;
	std::string fVertexModuleLabel;
	std::string fCalorimetryModuleLabel;

	calo::CalorimetryAlg fCalorimetryAlg;

};

proto::ECalibration::ECalibration(fhicl::ParameterSet const & p)
  :
  EDAnalyzer(p),
	fCalorimetryAlg(p.get<fhicl::ParameterSet>("CalorimetryAlg"))
 // More initializers here.
{
	// get a pointer to the geometry service provider
	fGeometry = &*(art::ServiceHandle<geo::Geometry>());
	
	reconfigure(p);
}

void proto::ECalibration::beginRun(const art::Run&)
{
	art::ServiceHandle<sim::LArG4Parameters> larParameters;
  fElectronsToGeV = 1./larParameters->GeVToElectrons();
}

void proto::ECalibration::beginJob()
{
	// access art's TFileService, which will handle creating and writing hists
	art::ServiceHandle<art::TFileService> tfs;

	fMomentumStartHist = tfs->make<TH1D>("mom",";particle Momentum (GeV);",100, 0.,    0.);

	fTree = tfs->make<TTree>("calibration","calibration tree");
	fTree->Branch("fEkinreco", &fEkinreco, "fEkinreco/D");
}

void proto::ECalibration::reconfigure(fhicl::ParameterSet const & p)
{
	fSimulationProducerLabel = p.get< std::string >("SimulationLabel");
  fHitsModuleLabel     =   p.get< std::string >("HitsModuleLabel");
  fTrackModuleLabel    =   p.get< std::string >("TrackModuleLabel");
  fShowerModuleLabel    =   p.get< std::string >("ShowerModuleLabel");
  fClusterModuleLabel  =   p.get< std::string >("ClusterModuleLabel");
  fVertexModuleLabel   =   p.get< std::string >("VertexModuleLabel");	
	fCalorimetryModuleLabel = p.get< std::string >("CalorimetryModuleLabel");
}

void proto::ECalibration::analyze(art::Event const & e)
{
  // Implementation of required member function here.
	ResetVars();

	fRun = e.run();
	fEvent = e.id().event();

	art::Handle< std::vector<simb::MCParticle> > particleHandle;
	if (e.getByLabel(fSimulationProducerLabel, particleHandle))
		art::fill_ptr_vector(fSimlist, particleHandle);

	std::map< int, const simb::MCParticle* > particleMap;

	for (auto const& particle : fSimlist)
	{
		
		fSimTrackID = particle->TrackId();	
	
		// add the address of the MCParticle to the map, with the track ID as the key
		particleMap[fSimTrackID] = &*particle;
	}

	fSimPdg = particleMap.begin()->second->PdgCode();
	fMomentumStartHist->Fill(particleMap.begin()->second->Momentum(0).P());
	if ((fSimPdg == 2212) && (particleMap.size() == 1)) 
		fStopping = true;
	if ((fSimPdg == 321) && (particleMap.begin()->second->EndProcess() == "Decay"))
	 	fDecaying = true;
	if ((fSimPdg == 221) && (particleMap.begin()->second->EndProcess() == "Decay"))
		fDecaying = true;

	// reco
	// hits
	art::Handle< std::vector<recob::Hit> > hitListHandle;
	if (e.getByLabel(fHitsModuleLabel, hitListHandle))
		art::fill_ptr_vector(fHitlist, hitListHandle);

	fEkinreco = GetEkreco(0);
	fTree->Fill();

	// tracks
	art::Handle< std::vector<recob::Track> > trackListHandle;
	if (e.getByLabel(fTrackModuleLabel, trackListHandle))
		art::fill_ptr_vector(fTracklist, trackListHandle);

	// vertices
	art::Handle< std::vector<recob::Vertex> > vtxListHandle;
	if (e.getByLabel(fVertexModuleLabel, vtxListHandle))
		art::fill_ptr_vector(fVertexlist, vtxListHandle);

	// showers
	art::Handle< std::vector<recob::Shower> > shsListHandle;
	if (e.getByLabel(fShowerModuleLabel, shsListHandle))
		art::fill_ptr_vector(fShslist, shsListHandle);
}

double proto::ECalibration::GetEkreco(double t0)
{
	if (!fHitlist.size()) return 0.0;

	double dqsum = 0.0;
	for (size_t h = 0; h < fHitlist.size(); ++h)
	{
		unsigned short plane = fHitlist[h]->WireID().Plane;
		if (plane != geo::kZ) continue;
	
		double dqadc = fHitlist[h]->Integral();
		if (!std::isnormal(dqadc) || (dqadc < 0)) continue;
	
		double dqel = fCalorimetryAlg.ElectronsFromADCArea(dqadc, plane);
		
		double tdrift = fHitlist[h]->PeakTime();
		double correllifetime = fCalorimetryAlg.LifetimeCorrection(tdrift, t0);

		double dq = dqel * correllifetime * fElectronsToGeV;
		if (!std::isnormal(dq) || (dq < 0)) continue;

		dqsum += dq; 
	}
	
	return dqsum; // should be corrected by average recombination factor.
}

void proto::ECalibration::ResetVars()
{
	fSimlist.clear();
	fHitlist.clear();
	fTracklist.clear();
	fVertexlist.clear();
	fShslist.clear();
	fSimPdg = 0;
	fStopping = false;
	fDecaying = false;
	fEkinreco = 0.0;
}

DEFINE_ART_MODULE(proto::ECalibration)
