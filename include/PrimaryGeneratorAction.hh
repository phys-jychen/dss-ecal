#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "HistoManager.hh"
#include "Config.hh"

class G4VPrimaryGenerator;
class G4Event;
class DetectorConstruction;
class HistoManager;
class Config;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction(DetectorConstruction*, HistoManager*, Config* c);    
    ~PrimaryGeneratorAction();

public:
    virtual void GeneratePrimaries(G4Event*);
    G4GeneralParticleSource* GetParticleGun()
    {
        return fGParticleSource;
    }

private:
    G4VPrimaryGenerator* HEPEvt;
//    G4VPrimaryGenerator* particleGun;
    G4bool useHEPEvt;
    G4GeneralParticleSource* fGParticleSource;
    DetectorConstruction* fDetector;
    HistoManager* fHistoManager_Particle;
    Config* config;

public:
    inline void SetHEPEvtGenerator(G4bool f)
    {
        useHEPEvt = f;
    }

    inline G4bool GetHEPEvtGenerator()
    {
        return useHEPEvt;
    }
};

#endif
