//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef NXRunAction_h
#define NXRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Run;

class NXRunAction : public G4UserRunAction
{
    public:
        NXRunAction();
        ~NXRunAction();

    public:
        void BeginOfRunAction(const G4Run*);
        void EndOfRunAction(const G4Run*);

        G4int CenterESofGamma[2000];
        G4int CenterESofPositron[2000];
        G4int CenterESofNegatron[2000];
        G4int NumofPrePointNotBoundary;
        G4int NumofPrePointIsBoundary;
        G4int OtherParticle;
        G4double EnergyofGammaContrb;
        G4double EnergyofNegatronContrb;
        G4double EnergyofPositronContrb;
        G4double EnergyofOtherParticleContrb;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif





