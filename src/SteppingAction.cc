#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4UnitsTable.hh"
//#include "G4EmSaturation.hh"

#include "SteppingAction.hh"

SteppingAction* SteppingAction::fgInstance = 0;
SteppingAction* SteppingAction::Instance()
{
    return fgInstance;
}

SteppingAction::SteppingAction(DetectorConstruction* det, EventAction* event) 
 : G4UserSteppingAction(),
   fVolume(0),
   fDetector(det), fEventAction_Step(event)                                         
{
    fgInstance = this;
    kineticEn = 0;
    volume1 = "none";
    volume2 = "none";
}

SteppingAction::~SteppingAction()
{ 
    fgInstance = 0;
}

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
//    G4EmSaturation* G4Em = new G4EmSaturation();
//    G4Em->SetVerbose(0);
//    G4double edep =   G4Em->VisibleEnergyDeposition(aStep);
    G4double edep = BirksAttenuation(aStep);
    G4int copyNo = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
    G4int pdgid = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
    G4int trackid = aStep->GetTrack()->GetTrackID();
    G4double time = aStep->GetPreStepPoint()->GetGlobalTime();
    G4String volumeName = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
    if (time > 150.0)
        return;

    // check if we are in scoring volume
    // collect energy and track length step by step
//    G4cout << volumeName << " " << G4int (volumeName == "LYSO") << " " << edep << G4endl;
    if (volumeName == "LYSO")
        fEventAction_Step->AddEcalHit(copyNo, edep, time, pdgid, trackid);
}
 
void SteppingAction::Reset()
{
//    fEnergy = 0.0;
}

G4double SteppingAction::BirksAttenuation(const G4Step* aStep)
{
    //Example of Birk attenuation law in organic scintillators.
    //adapted from Geant3 PHYS337. See MIN 80 (1970) 239-244
    //
    G4Material* material = aStep->GetTrack()->GetMaterial();
    G4double birk1       = material->GetIonisation()->GetBirksConstant();
    G4double destep      = aStep->GetTotalEnergyDeposit();
    G4double stepl       = aStep->GetStepLength();  
    G4double charge      = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
    //
    G4double response = destep;
    if (birk1 * destep * stepl * charge != 0.0)
        response = destep / (1.0 + birk1 * destep / stepl);
    return response;
}
