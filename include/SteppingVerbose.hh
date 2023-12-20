#ifndef SteppingVerbose_h
#define SteppingVerbose_h 1

#include "G4SteppingVerbose.hh"
#include "HistoManager.hh"

class SteppingVerbose : public G4SteppingVerbose
{
public:   
    SteppingVerbose();
    ~SteppingVerbose();
 
    virtual void TrackingStarted();
    virtual void StepInfo();
  
    //ParticleInfo fParticleInfo_Step;
    G4double fEdep;
};

#endif
