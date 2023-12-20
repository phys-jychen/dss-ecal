#include "PhysicsList.hh"
#include "G4UnitsTable.hh"
#include "G4ParticleTypes.hh"
#include "G4IonConstructor.hh"
#include "G4PhysicsListHelper.hh"
#include "G4RadioactiveDecay.hh"
#include "G4UAtomicDeexcitation.hh"
#include "G4LossTableManager.hh"
#include "G4SystemOfUnits.hh"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4PhysicsListHelper.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4RayleighScattering.hh"
#include "G4KleinNishinaModel.hh"

#include "G4eMultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuMultipleScattering.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hMultipleScattering.hh"
#include "G4hIonisation.hh"
#include "G4hBremsstrahlung.hh"
#include "G4hPairProduction.hh"

#include "G4ionIonisation.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4NuclearStopping.hh"

// Particles

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

PhysicsList::PhysicsList()
 : G4VUserPhysicsList()
{
    // Add new units for radioActive decays
    const G4double minute = 60 * second;
    const G4double hour   = 60 * minute;
    const G4double day    = 24 * hour;
    const G4double year   = 365 * day;
    new G4UnitDefinition("minute", "min", "Time", minute);
    new G4UnitDefinition("hour",   "h",   "Time", hour);
    new G4UnitDefinition("day",    "d",   "Time", day);
    new G4UnitDefinition("year",   "y",   "Time", year);
}

PhysicsList::~PhysicsList() {}

void PhysicsList::ConstructParticle()
{
    G4BosonConstructor  pBosonConstructor;
    pBosonConstructor.ConstructParticle();
  
    G4LeptonConstructor pLeptonConstructor;
    pLeptonConstructor.ConstructParticle();
  
    G4MesonConstructor pMesonConstructor;
    pMesonConstructor.ConstructParticle();
  
    G4BaryonConstructor pBaryonConstructor;
    pBaryonConstructor.ConstructParticle();
  
    G4IonConstructor pIonConstructor;
    pIonConstructor.ConstructParticle();
  
    G4ShortLivedConstructor pShortLivedConstructor;
    pShortLivedConstructor.ConstructParticle(); 

    /*
    // Pseudo-particles
    G4Geantino::GeantinoDefinition();
  
    // Gamma
    G4Gamma::GammaDefinition();

    // Leptons
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();

    G4NeutrinoE::NeutrinoEDefinition();
    G4AntiNeutrinoE::AntiNeutrinoEDefinition();
  
    // Baryons
    G4Proton::ProtonDefinition();
    G4Neutron::NeutronDefinition();  

    // Ions
    G4IonConstructor iConstructor;
    iConstructor.ConstructParticle();  
    */
}

void PhysicsList::ConstructProcess()
{
    AddTransportation();

    /*
    G4RadioactiveDecay* radioactiveDecay = new G4RadioactiveDecay();

    radioactiveDecay->SetICM(true);                //Internal Conversion
    radioactiveDecay->SetARM(false);               //Atomic Rearrangement
  
    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();  
    ph->RegisterProcess(radioactiveDecay, G4GenericIon::GenericIon());
      
    // De-excitation (in case of Atomic Rearrangement)
    G4UAtomicDeexcitation* de = new G4UAtomicDeexcitation();
    de->SetFluo(true);
    de->SetAuger(true);   
    de->SetPIXE(false);  
    G4LossTableManager::Instance()->SetAtomDeexcitation(de);  
    */

    ConstructEMProcess();
    G4cout << "******************** 33333333333333333333 ********************" << G4endl;
}

void PhysicsList::SetCuts()
{
    SetCutsWithDefault();    // Define all production cuts for gamma, electron and positrons
}

void PhysicsList::ConstructEMProcess()
{
    G4cout << "******************** 22222222222222222222 ********************" << G4endl;
  
    // Add standard EM Processes

//    ParticleIterator *theParticleIterator = new ParticleIterator();
    G4ParticleTable::G4PTblDicIterator*
    theParticleIterator = theParticleTable-> GetIterator();

    theParticleIterator->reset();
    while( (*theParticleIterator)() )
    {
        G4ParticleDefinition* particle = theParticleIterator->value();
        G4String particleName = particle->GetParticleName();
        G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
     
        if (particleName == "gamma")
        {
            ////ph->RegisterProcess(new G4RayleighScattering, particle);      
            ph->RegisterProcess(new G4PhotoElectricEffect, particle);      
            G4ComptonScattering* cs = new G4ComptonScattering;
            cs->SetEmModel(new G4KleinNishinaModel());
            ph->RegisterProcess(cs, particle);
            ph->RegisterProcess(new G4GammaConversion, particle);
        }
        else if (particleName == "e-")
        {
            ph->RegisterProcess(new G4eMultipleScattering(), particle);
            G4eIonisation* eIoni = new G4eIonisation();
            eIoni->SetStepFunction(0.1, 100 * um);
            ph->RegisterProcess(eIoni, particle);
            ph->RegisterProcess(new G4eBremsstrahlung(), particle);      
        }
        else if (particleName == "e+")
        {
            ph->RegisterProcess(new G4eMultipleScattering(), particle);
            G4eIonisation* eIoni = new G4eIonisation();
            eIoni->SetStepFunction(0.1, 100 * um);
            ph->RegisterProcess(eIoni, particle);
            ph->RegisterProcess(new G4eBremsstrahlung(), particle);
            ph->RegisterProcess(new G4eplusAnnihilation(), particle);    
        }
        else if (particleName == "mu+" || particleName == "mu-")
        {
            ph->RegisterProcess(new G4MuMultipleScattering(), particle);
            G4MuIonisation* muIoni = new G4MuIonisation();
            muIoni->SetStepFunction(0.1, 50 * um);
            ph->RegisterProcess(muIoni, particle);
            ph->RegisterProcess(new G4MuBremsstrahlung(), particle);
            ph->RegisterProcess(new G4MuPairProduction(), particle);
        }
        else if (particleName == "proton" || particleName == "pi-" || particleName == "pi+")
        {
            ph->RegisterProcess(new G4hMultipleScattering(), particle);      
            G4hIonisation* hIoni = new G4hIonisation();
            hIoni->SetStepFunction(0.1, 20 * um);
            ph->RegisterProcess(hIoni, particle);
            ph->RegisterProcess(new G4hBremsstrahlung(), particle);
            ph->RegisterProcess(new G4hPairProduction(), particle);            
        }
        else if (particleName == "alpha" || particleName == "He3")
        {
            ph->RegisterProcess(new G4hMultipleScattering(), particle);           
            G4ionIonisation* ionIoni = new G4ionIonisation();
            ionIoni->SetStepFunction(0.1, 1 * um);
            ph->RegisterProcess(ionIoni, particle);
            ph->RegisterProcess(new G4NuclearStopping(), particle);      
        }
        else if (particleName == "GenericIon")
        {
            ph->RegisterProcess(new G4hMultipleScattering(), particle);          
            G4ionIonisation* ionIoni = new G4ionIonisation();
            ionIoni->SetEmModel(new G4IonParametrisedLossModel());
            ionIoni->SetStepFunction(0.1, 1 * um);
            ph->RegisterProcess(ionIoni, particle);
            ph->RegisterProcess(new G4NuclearStopping(), particle);                   
        }
        else if (!particle->IsShortLived() && particle->GetPDGCharge() != 0.0 && particle->GetParticleName() != "chargedgeantino")
        {
            // All others charged particles except geantino
            ph->RegisterProcess(new G4hMultipleScattering(), particle);
            ph->RegisterProcess(new G4hIonisation(), particle);
    }
  }
}