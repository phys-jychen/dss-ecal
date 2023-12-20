#include "G4Track.hh"
#include "G4ParticleTypes.hh"
#include "G4RunManager.hh"

#include "SteppingAction.hh"
#include "RunAction.hh"
#include "TrackingAction.hh"
#include "HistoManager.hh"
//#include "Run.hh"
#include "EventAction.hh"

TrackingAction::TrackingAction(RunAction* runAct, EventAction* EA, Config* c)
 : G4UserTrackingAction(),
   fRun(runAct),fEvent(EA),
   fFullChain(true),config(c)
{
//    fSteppingVerbose_Tracking = new SteppingVerbose();
}

TrackingAction::~TrackingAction() {}

void TrackingAction::PreUserTrackingAction(const G4Track* track)
{
    fEvent->fStepTag = 0;
//    Run* run = static_cast<Run*>(G4RunManager::GetRunManager()->GetCurrentRun());

    G4ParticleDefinition* particle = track->GetDefinition();
    G4String name = particle->GetParticleName();
    G4int ID = track->GetTrackID();
    G4double Ekin = track->GetKineticEnergy();
    fParticleEnCode = particle->GetPDGEncoding();
}
  
void TrackingAction::PostUserTrackingAction(const G4Track* track) {}
