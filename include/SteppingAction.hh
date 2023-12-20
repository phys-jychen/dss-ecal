#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "HistoManager.hh"
#include "G4GeneralParticleSource.hh"

class DetectorConstruction;
class EventAction;
class G4LogicalVolume;
class HistoManager;

class SteppingAction : public G4UserSteppingAction
{
public:
    SteppingAction(DetectorConstruction*, EventAction*);
    //SteppingAction();
    virtual ~SteppingAction();
  
    static SteppingAction* Instance();

    virtual void UserSteppingAction(const G4Step*);
    
    void Reset();

    // Set methods
    void SetVolume(G4LogicalVolume* volume)
    {
        fVolume = volume;
    }

    // get methods
    G4LogicalVolume* GetVolume() const
    {
        return fVolume;
    }

    //G4double GetEnergy() const { return fEnergy;}
    double preEnergy;

private:
    static SteppingAction* fgInstance;
    G4LogicalVolume* fVolume;
    DetectorConstruction* fDetector;
    EventAction*          fEventAction_Step;  
    G4double BirksAttenuation(const G4Step* aStep);
    G4GeneralParticleSource* fGParticleSource;
    G4double kineticEn;
    G4String volume1;
    G4String volume2;
//    G4double fEnergy; 
};

#endif
