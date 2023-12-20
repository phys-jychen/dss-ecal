#include "G4Event.hh"
#include "G4HEPEvtInterface.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Geantino.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "G4GeneralParticleSource.hh"

#include "HistoManager.hh"
#include "PrimaryGeneratorAction.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* det, HistoManager* histo, Config* c)
 : G4VUserPrimaryGeneratorAction(),
   fGParticleSource(),
   fDetector(det),
   fHistoManager_Particle(histo),
   config(c)
{
//    const char* filename = "pythia_event.data";
//    HEPEvt = new G4HEPEvtInterface();

    fGParticleSource  = new G4GeneralParticleSource();
    /*
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle(config->conf["Source"]["particle"].as<std::string>());

    auto fParticleGun = fGParticleSource->GetCurrentSource();
    fParticleGun->SetParticleDefinition(particle);

    fParticleGun->GetAngDist()->SetAngDistType(config->conf["Source"]["ang_type"].as<std::string>());
    std::vector<G4double> gps_direction = config->conf["Source"]["direction"].as<std::vector<G4double>>();
    fParticleGun->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(gps_direction.at(0), gps_direction.at(1), gps_direction.at(2)));

    std::string ene_type = config->conf["Source"]["ene_type"].as<std::string>();
    fParticleGun->GetEneDist()->SetEnergyDisType(ene_type);
    if (ene_type == "Mono")
        fParticleGun->GetEneDist()->SetMonoEnergy(config->conf["Source"]["energy"].as<G4double>() * 1000.0);

    fParticleGun->GetPosDist()->SetPosDisType(config->conf["Source"]["pos_type"].as<std::string>());
    std::vector<G4double> gps_position = config->conf["Source"]["position"].as<std::vector<G4double>>();
    fParticleGun->GetPosDist()->SetCentreCoords(G4ThreeVector(gps_position.at(0), gps_position.at(1), gps_position.at(2)));
    */

    useHEPEvt = false;
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
//    delete HEPEvt;
    delete fGParticleSource;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    // Set random particle position
    // Create vertex
    fHistoManager_Particle->fParticleInfo.reset();
    if (useHEPEvt)
    {
//        HEPEvt->GeneratePrimaryVertex(anEvent);
//        G4cout << " ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ " << G4endl;
    }
    else
    {
        fGParticleSource->GeneratePrimaryVertex(anEvent);
//        G4cout << " @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ " << G4endl;
//        G4cout << angle << " : " << position.x() << ", " << position.y() << ", " << position.z() << G4endl;
    }
}