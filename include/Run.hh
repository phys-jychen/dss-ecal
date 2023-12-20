#ifndef Run_h
#define Run_h 1

#include "G4Run.hh"
#include "G4VProcess.hh"
#include "globals.hh"
#include <map>

class G4ParticleDefinition;

class Run : public G4Run
{
public:
    Run();
    ~Run();

public:
    void ParticleCount(G4String, G4double);
    void Balance(G4double,G4double);
    void EventTiming(G4double);
    void PrimaryTiming(G4double);
        
    void SetPrimary(G4ParticleDefinition* particle, G4double energy);
    void EndOfRun(); 
            
//    virtual void Merge(const G4Run*);

private:    
    struct ParticleData
    {
     ParticleData()
       : fCount(0), fEmean(0.0), fEmin(0.0), fEmax(0.0) {}
     ParticleData(G4int count, G4double ekin, G4double emin, G4double emax)
       : fCount(count), fEmean(ekin), fEmin(emin), fEmax(emax) {}
     G4int     fCount;
     G4double  fEmean;
     G4double  fEmin;
     G4double  fEmax;
    };
     
  private: 
    G4ParticleDefinition*  fParticle;
    G4double  fEkin;
             
    std::map<G4String,ParticleData>  fParticleDataMap;    
    G4int    fDecayCount, fTimeCount;
    G4double fEkinTot[3];
    G4double fPbalance[3];
    G4double fEventTime[3];
    G4double fPrimaryTime;
};

#endif
