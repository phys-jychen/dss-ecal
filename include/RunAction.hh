#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "Config.hh"
#include <map>

class G4Run;
class HistoManager;
class PrimaryGeneratorAction;
class Config;

class RunAction : public G4UserRunAction
{
public:
    RunAction(PrimaryGeneratorAction*,HistoManager* histo,Config* c);
    ~RunAction();
   
    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);
    
    void ParticleCount(G4String, G4double);
    void Balance(G4double,G4double);
    void EventTiming(G4double);
    void PrimaryTiming(G4double);
    
private:
    PrimaryGeneratorAction* fPrimary;
    HistoManager*           fHistoManager;
    Config*					config;
    
    std::map<G4String,G4int> fParticleCount;
    std::map<G4String,G4double> fEmean;
    std::map<G4String,G4double> fEmin;
    std::map<G4String,G4double> fEmax;
    G4int    fDecayCount, fTimeCount;
    G4double fEkinTot[3];
    G4double fPbalance[3];
    G4double fEventTime[3];
    G4double fPrimaryTime;                        
//    G4double fPrimaryEnergy;                        
};

#endif
