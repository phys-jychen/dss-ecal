#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "SteppingAction.hh"
#include "Config.hh"

class Config;
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction(Config* c);
    ~DetectorConstruction();

    virtual     
    G4VPhysicalVolume* Construct();
                        
    G4double GetWorldSize()
    {
        return fWorldSize;
    }

	G4VPhysicalVolume* GetphysiWorld()
	{
	    return this->physiWorld;
	}
    
private:
    G4double fWorldSize;
	G4LogicalVolume* logicWorld;
	G4VPhysicalVolume* ConstructWorld(); 
	G4VPhysicalVolume* physiWorld;
    G4VisAttributes* visAttributes;
	Config* config;
};

#endif
