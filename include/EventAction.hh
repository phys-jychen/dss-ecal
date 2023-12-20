#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "G4EventManager.hh"
#include "globals.hh"
#include "HistoManager.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "DetectorConstruction.hh"
#include "Config.hh"
#include "TMath.h"
#include "TRandom.h"

class EventAction : public G4UserEventAction
{
public:
    EventAction(HistoManager*, Config* c);
    ~EventAction();

public:
    virtual void BeginOfEventAction(const G4Event*);
    virtual void   EndOfEventAction(const G4Event*);
    G4int fStepTag;

    void SetPrintModulo(G4int val)
    {
        fPrintModulo = val;
    }

    void AddDecayChain(G4String val)
    {
        fDecayChain += val;
    }

    void AddEcalHit(const G4int& copyNo, const G4double& edep, const G4double& time, const G4int& pdgid, const G4int& trackid);

    /*
    void AddCrystalEnDep(G4int copyNo, G4double edep)
    {
        for (size_t i_copyNo = 0; i_copyNo != (fHistoManager_Event->fParticleInfo.fCrystalID.size()); ++i_copyNo)
            if (fHistoManager_Event->fParticleInfo.fCrystalID.at(i_copyNo) == copyNo)
            {
                fHistoManager_Event->fParticleInfo.fEnergyDep.at(i_copyNo) = fHistoManager_Event->fParticleInfo.fEnergyDep.at(i_copyNo) + edep;
                return;
            }
        fHistoManager_Event->fParticleInfo.fCrystalID.emplace_back(copyNo);
        fHistoManager_Event->fParticleInfo.fEnergyDep.emplace_back(edep);
    }
    */

private:
    Double_t SiPMDigi(const Double_t& edep) const;
    G4double      fEventEdep;
    G4int         fPrintModulo;
    G4String      fDecayChain;
    HistoManager* fHistoManager_Event;
    Config*       config;
    G4GeneralParticleSource* fGParticleSource;
};

#endif
