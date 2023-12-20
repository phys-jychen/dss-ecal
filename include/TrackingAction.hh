#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "globals.hh"
#include "HistoManager.hh"
#include "SteppingAction.hh"
#include "Config.hh"

//class SteppingVerbose;
class RunAction;
class EventAction;
class ParticleInfo;
class Config;

class TrackingAction : public G4UserTrackingAction
{
public:  
    TrackingAction(RunAction*,EventAction*,Config *c);
    ~TrackingAction();
   
    virtual void  PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);
    
    void SetFullChain(G4bool flag) { fFullChain = flag;};
  
    //ParticleInfo fParticleInfo_Track;
    void AddEdep(G4double edep)
    {
        fTrackEdep +=edep;
        //G4cout<<"TrackingAction------ "<<fTrackEdep<<G4endl;
    }
  
private:
    RunAction* fRun;
    EventAction*        fEvent;
    Config*		config;
    
    G4double fCharge, fBaryonNo, fMass;
    G4double  fParticleEnCode;        
    G4double fTrackEdep;
    G4bool   fFullChain;
    ParticleInfo fParticleInfo_Tracking;
    //SteppingAction fSteppingAction_Tracking;
};

#endif
