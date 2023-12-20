#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4GlobalMagFieldMessenger.hh"

#include "DetectorConstruction.hh"
#include "SteppingAction.hh"

DetectorConstruction::DetectorConstruction(Config* c)
 : G4VUserDetectorConstruction(),
   config(c)
{}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    visAttributes = new G4VisAttributes(G4Colour(0.9, 0.0, 0.0));
    visAttributes->SetVisibility(false);

    physiWorld = ConstructWorld();

    G4NistManager* nistManager = G4NistManager::Instance();

    // Elements and basic materials
    G4Element* elH = nistManager->FindOrBuildElement("H");
    G4Element* elC = nistManager->FindOrBuildElement("C");
    G4Element* elO = nistManager->FindOrBuildElement("O");
    G4Element* elSi = nistManager->FindOrBuildElement("Si");
    G4Element* elY = nistManager->FindOrBuildElement("Y");
    G4Element* elLu = nistManager->FindOrBuildElement("Lu");
    G4Material* C = nistManager->FindOrBuildMaterial("G4_C");
    G4Material* Si = nistManager->FindOrBuildMaterial("G4_Si");

    // Crystal: LYSO
    G4Material* LSO = new G4Material("LSO", 7.4 * g / cm3, 3);
    LSO->AddElement(elLu, 2);
    LSO->AddElement(elSi, 2);
    LSO->AddElement(elO, 5);
    G4Material* YSO = new G4Material("YSO", 4.5 * g / cm3, 3);
    YSO->AddElement(elY, 2);
    YSO->AddElement(elSi, 1);
    YSO->AddElement(elO, 5);
    G4Material* LYSO = new G4Material("LYSO", 7.1 * g / cm3, 2);
    LYSO->AddMaterial(LSO, 90 * perCent);
    LYSO->AddMaterial(YSO, 10 * perCent);

    // PCB: FR4
    G4Material* quartz = nistManager->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    G4Material* epoxy = new G4Material("epoxy", 1.3 * g / cm3, 3);
    epoxy->AddElement(elC, 15);
    epoxy->AddElement(elH, 44);
    epoxy->AddElement(elO, 7);
    G4Material* FR4 = new G4Material("FR4", 1.86 * g / cm3, 2);
    FR4->AddMaterial(quartz, 52.8 * perCent);
    FR4->AddMaterial(epoxy, 47.2 * perCent);

    // SiPM: Si
    G4Material* SiPM = Si;

    // Wrapper: ESR
    G4Material* ESR = new G4Material("ESR", 1.38 * g / cm3, 3);
    ESR->AddElement(elC, 10);
    ESR->AddElement(elH, 8);
    ESR->AddElement(elO, 4);

    // Gap: carbon
    G4Material* sustain = C;

    // Parameters from YAML file
    G4int nCrystalInLayer = config->conf["ECAL"]["nCrystalInLayer"].as<G4int>();
    G4int nLayer = config->conf["ECAL"]["nLayer"].as<G4int>();
    G4double crystalWidth = config->conf["ECAL"]["crystalWidth"].as<G4double>() * cm;
    G4double crystalThick = config->conf["ECAL"]["crystalThick"].as<G4double>() * cm;
    G4double SiPMLength = config->conf["ECAL"]["SiPMLength"].as<G4double>() * mm;
    G4double SiPMWidth = config->conf["ECAL"]["SiPMWidth"].as<G4double>() * mm;
    G4bool constructPCB = config->conf["ECAL"]["constructPCB"].as<G4bool>();
    G4double defaultPCBThick = config->conf["ECAL"]["PCBThick"].as<G4double>() * mm;

    // Parameters of the components
    G4double crystalBasicLength = nCrystalInLayer * crystalWidth;
    G4double ESRThick = 0.3 * mm;
    G4double gap = 0.1 * mm;
    G4double edgeBias = ESRThick + 0.5 * gap;

    G4double crystalLength = crystalBasicLength + (nCrystalInLayer - 1) * (2 * ESRThick + gap);
    G4double ESROutLength = crystalLength + 2 * ESRThick;
    G4double ESROutWidth = crystalWidth + 2 * ESRThick;
    G4double ESROutThick = crystalThick + 2 * ESRThick;
    G4double SiPMThick = 0.6 * mm;
    G4double PCBLength = ESROutLength;
    G4double PCBWidth = ESROutWidth;
    G4double PCBThick = constructPCB ? defaultPCBThick : 0 * mm;
    G4double gapOutLength = ESROutLength + gap;
    G4double gapOutWidth = ESROutWidth + gap;
    G4double gapOutThick = ESROutThick + SiPMThick + PCBThick + gap;

    G4double thickness = gapOutThick;

    G4double crystalPositionZ = 0.5 * ESROutThick;
    G4double ESRPositionZ = crystalPositionZ;
    G4double SiPMPositionZ = ESRPositionZ + 0.5 * (ESROutThick + SiPMThick);
    G4double PCBPositionZ = SiPMPositionZ + 0.5 * (SiPMThick + PCBThick);
    G4double gapPositionZ = 0.5 * gapOutThick;

    G4bool checkOverlap = false;    // No overlap checking triggered

    // Crystal & ESR & SiPM & PCB
    G4Box* solidCrystal = new G4Box("LYSO",                                                          // Name
                                    0.5 * crystalLength, 0.5 * crystalWidth, 0.5 * crystalThick);    // Size
    G4Box* solidESROut = new G4Box("ESR_Out",                                                    // Name
                                   0.5 * ESROutLength, 0.5 * ESROutWidth, 0.5 * ESROutThick);    // Size
    G4Box* solidESRIn = new G4Box("ESR_In",                                                        // Name
                                  0.5 * crystalLength, 0.5 * crystalWidth, 0.5 * crystalThick);    // Size
    G4SubtractionSolid* solidESR = new G4SubtractionSolid("ESR",          // Name
                                                          solidESROut,    // Minuend
                                                          solidESRIn);    // Subtrahend
    G4Box* solidSiPM = new G4Box("SiPM",                                                 // Name
                                 0.5 * SiPMLength, 0.5 * SiPMWidth, 0.5 * SiPMThick);    // Size

    G4LogicalVolume* logicCrystal = new G4LogicalVolume(solidCrystal,    // Solid
                                                        LYSO,            // Material
                                                        "LYSO");         // Name
    G4LogicalVolume* logicESR = new G4LogicalVolume(solidESR,    // Solid
                                                    ESR,         // Material
                                                    "ESR");      // Name
    G4LogicalVolume* logicSiPM = new G4LogicalVolume(solidSiPM,    // Solid
                                                     SiPM,         // Material
                                                     "SiPM");      // Name

    G4Box* solidPCB = nullptr;
    G4LogicalVolume* logicPCB = nullptr;
    if (constructPCB)
    {
        solidPCB = new G4Box("FR4",                                               // Name
                             0.5 * PCBLength, 0.5 * PCBWidth, 0.5 * PCBThick);    // Size
        logicPCB = new G4LogicalVolume(solidPCB,    // Solid
                                       FR4,         // Material
                                       "FR4");      // Name
    }

    for (G4int i_layer = 0; i_layer < nLayer; i_layer++)
    {
        G4RotationMatrix rot;
        rot.rotateZ(90 * deg);
        if (i_layer % 2 == 0)
        {
            for (G4int i_y = 0; i_y < nCrystalInLayer; i_y++)
            {
                new G4PVPlacement(0,                              // No rotation
                                  G4ThreeVector(0,
                                                -0.5 * crystalLength - edgeBias + (0.5 + i_y) * gapOutWidth,
                                                i_layer * thickness + crystalPositionZ),
                                  logicCrystal,                   // Logical volume
                                  "LYSO",                         // Name
                                  logicWorld,                     // Mother volume
                                  false,                          // No Boolean operation
                                  10000 * i_layer + 100 * i_y,    // Copy number
                                  checkOverlap);
                new G4PVPlacement(0,             // No rotation
                                  G4ThreeVector(0,
                                                -0.5 * crystalLength - edgeBias + (0.5 + i_y) * gapOutWidth,
                                                i_layer * thickness + ESRPositionZ),
                                  logicESR,      // Logical volume
                                  "ESR",         // Name
                                  logicWorld,    // Mother volume
                                  false,         // No Boolean operation
                                  -1,            // Copy number
                                  checkOverlap);
                new G4PVPlacement(0,             // No rotation
                                  G4ThreeVector(0,
                                                -0.5 * crystalLength - edgeBias + (0.5 + i_y) * gapOutWidth,
                                                i_layer * thickness + SiPMPositionZ),
                                  logicSiPM,     // Logical volume
                                  "SiPM",        // Name
                                  logicWorld,    // Mother volume
                                  false,         // No Boolean operation
                                  -1,            // Copy number
                                  checkOverlap);
                if (constructPCB)
                    new G4PVPlacement(0,             // No rotation
                                      G4ThreeVector(0,
                                                    -0.5 * crystalLength - edgeBias + (0.5 + i_y) * gapOutWidth,
                                                    i_layer * thickness + PCBPositionZ),
                                      logicPCB,      // Logical volume
                                      "FR4",         // Name
                                      logicWorld,    // Mother volume
                                      false,         // No Boolean operation
                                      -1,            // Copy number
                                      checkOverlap);
            }
        }
        else if (i_layer % 2 == 1)
        {
            for (G4int i_x = 0; i_x < nCrystalInLayer; i_x++)
            {
                new G4PVPlacement(G4Transform3D(rot,        // Rotation matrix
                                                G4ThreeVector(-0.5 * crystalLength - edgeBias + (0.5 + i_x) * gapOutWidth,
                                                              0,
                                                              i_layer * thickness + crystalPositionZ)),
                                  logicCrystal,             // Logical volume
                                  "LYSO",                   // Name
                                  logicWorld,               // Mother volume
                                  false,                    // No Boolean operation
                                  10000 * i_layer + i_x,    // Copy number
                                  checkOverlap);
                new G4PVPlacement(G4Transform3D(rot,    // Rotation matrix
                                                G4ThreeVector(-0.5 * crystalLength - edgeBias + (0.5 + i_x) * gapOutWidth,
                                                              0,
                                                              i_layer * thickness + ESRPositionZ)),
                                  logicESR,             // Logiacl volume
                                  "ESR",                // Name
                                  logicWorld,           // Mother volume
                                  false,                // No Boolean operation
                                  -1,                   // Copy number
                                  checkOverlap);
                new G4PVPlacement(0,             // No rotation
                                  G4ThreeVector(-0.5 * crystalLength - edgeBias + (0.5 + i_x) * gapOutWidth,
                                                0,
                                                i_layer * thickness + SiPMPositionZ),
                                  logicSiPM,     // Logical volume
                                  "SiPM",        // Logical volume
                                  logicWorld,    // Mother volume
                                  false,         // No Boolean operation
                                  -1,            // Copy number
                                  checkOverlap);
                if (constructPCB)
                    new G4PVPlacement(G4Transform3D(rot,    // Rotation matrix
                                                    G4ThreeVector(-0.5 * crystalLength - edgeBias + (0.5 + i_x) * gapOutWidth,
                                                                  0,
                                                                  i_layer * thickness + PCBPositionZ)),
                                      logicPCB,             // Logical volume
                                      "FR4",                // Name
                                      logicWorld,           // Mother volume
                                      false,                // No Boolean operation
                                      -1,                   // Copy number
                                      checkOverlap);
            }
        }
    }

    // Gap
    G4Box* solidGapOut = new G4Box("Gap_Out",                                                    // Name
                                   0.5 * gapOutLength, 0.5 * gapOutWidth, 0.5 * gapOutThick);    // Size
    G4Box* solidGapIn = new G4Box("Gap_In",                                                     // Name
                                  0.5 * ESROutLength, 0.5 * ESROutWidth, 0.5 * ESROutThick);    // Size
    G4SubtractionSolid* solidGap = new G4SubtractionSolid("Gap",          // Name
                                                          solidGapOut,    // Minuend
                                                          solidGapIn);    // Subtrahend

    G4LogicalVolume* logicGap = new G4LogicalVolume(solidGap,    // Solid
                                                    sustain,     // Material
                                                    "Gap");      // Name

    for (G4int i_layer = 0; i_layer < nLayer; i_layer++)
    {
        G4RotationMatrix rot;
        rot.rotateZ(90 * deg);
        if (i_layer % 2 == 0)
        {
            for (G4int i_y = 0; i_y < nCrystalInLayer; i_y++)
            {
                new G4PVPlacement(0,             // No rotation
                                  G4ThreeVector(0,
                                                -0.5 * crystalLength - edgeBias + (0.5 + i_y) * gapOutWidth,
                                                i_layer * thickness + gapPositionZ),
                                  logicGap,      // Logical volume
                                  "Gap",         // Name
                                  logicWorld,    // Mother volume
                                  false,         // No Boolean operation
                                  -1,            // Copy number
                                  checkOverlap);
            }
        }
        else if (i_layer % 2 == 1)
        {
            for (G4int i_x = 0; i_x < nCrystalInLayer; i_x++)
            {
                new G4PVPlacement(G4Transform3D(rot,    // Rotation matrix
                                                G4ThreeVector(-0.5 * crystalLength - edgeBias + (0.5 + i_x) * gapOutWidth,
                                                              0,
                                                              i_layer * thickness + gapPositionZ)),
                                  logicGap,             // Logical volume
                                  "Gap",                // Name
                                  logicWorld,           // Mother volume
                                  false,                // No Boolean operation
                                  -1,                   // Copy number
                                  checkOverlap);
            }
        }
    }

//    logicAbsorber->SetVisAttributes(visAttributes);
//    (0.9, 0.9, 0.0)yellow

//    visAttributes = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));    // Green
//    logicPCB->SetVisAttributes(visAttributes);

//    visAttributes = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9));    // White
//    logicCrystal->SetVisAttributes(visAttributes);

//    SteppingAction* steppingAction = SteppingAction::Instance();

    return physiWorld;
}

G4VPhysicalVolume* DetectorConstruction::ConstructWorld()
{
    G4NistManager* nistManager = G4NistManager::Instance();
    G4Material* Vacuum = nistManager->FindOrBuildMaterial("G4_Galactic");
    G4bool checkOverlaps = false;

    // Full sphere shape
    G4double solidWorld_rmax = 200 * cm;
    G4Orb* solidWorld = new G4Orb("World",             // Name
                                  solidWorld_rmax);    // Size

//    G4LogicalVolume*
    logicWorld = new G4LogicalVolume(solidWorld,    // Solid
                                     Vacuum,        // Material
                                     "World");      // Name
    G4VPhysicalVolume* physicalWorld = new G4PVPlacement(0,                  // No rotation
                                                         G4ThreeVector(),    // At (0,0,0)
                                                         logicWorld,         // Logical volume
                                                         "World",            // Name
                                                         0,                  // Mother volume
                                                         false,              // No boolean operation
                                                         0,                  // Copy number
                                                         checkOverlaps);
    logicWorld->SetVisAttributes(visAttributes);

    return physicalWorld;
}
