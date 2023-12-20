#include "HistoManager.hh"
#include "G4UnitsTable.hh"
#include <TTree.h>
#include <TFile.h>

HistoManager::HistoManager(const char* foutname, const G4bool& savegeo)
  : fRootFile(0), fNtuple(0), fSaveGeo(savegeo)
{
    fOutName = foutname;
}

HistoManager::~HistoManager()
{
    delete G4AnalysisManager::Instance();
}

void HistoManager::book()
{
    G4cout << "----------> Creating ROOT file < ----------" << G4endl << G4endl;
    fRootFile = new TFile(fOutName.c_str(), "RECREATE");
    fNtuple = new TTree("dp", "MC events");

    /*
    // ECAL
    fNtuple->Branch("ecal_psdid",          &fParticleInfo.fecal_psdid);
    fNtuple->Branch("ecal_energy",         &fParticleInfo.fecal_energy);
    fNtuple->Branch("ecal_pdgid",          &fParticleInfo.fecal_pdgid);
    fNtuple->Branch("ecal_trackid",        &fParticleInfo.fecal_trackid);
    fNtuple->Branch("ecal_x",              &fParticleInfo.fecal_x);
    fNtuple->Branch("ecal_y",              &fParticleInfo.fecal_y);
    fNtuple->Branch("ecal_z",              &fParticleInfo.fecal_z);
    fNtuple->Branch("ecal_time",           &fParticleInfo.fecal_time);
    fNtuple->Branch("ecal_cellid",         &fParticleInfo.fecal_cellid);
    fNtuple->Branch("ecal_celle",          &fParticleInfo.fecal_celle);
    fNtuple->Branch("ecal_cellx",          &fParticleInfo.fecal_cellx);
    fNtuple->Branch("ecal_celly",          &fParticleInfo.fecal_celly);
    fNtuple->Branch("ecal_cellz",          &fParticleInfo.fecal_cellz);
    */
    fNtuple->Branch("CellID",              &fParticleInfo.fecal_cellid);
    fNtuple->Branch("Hit_Energy",          &fParticleInfo.fecal_celle);
    fNtuple->Branch("Hit_X",               &fParticleInfo.fecal_cellx);
    fNtuple->Branch("Hit_Y",               &fParticleInfo.fecal_celly);
    fNtuple->Branch("Hit_Z",               &fParticleInfo.fecal_cellz);
}

void HistoManager::save()
{
    if (fSaveGeo)
    {
        gSystem->Load("libGeom");
        TGeoManager::Import("dss-ecal.gdml");
        gGeoManager->Write("dss_ecal");
        std::remove("dss-ecal.gdml");
    }

    fNtuple->Write("", TObject::kOverwrite);
    fRootFile->Close();
    G4cout << "----------> Closing ROOT file <----------" << G4endl << G4endl;
}
