//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file analysis/shared/src/SteppingAction.cc
/// \brief Implementation of the SteppingAction class
//
//
// $Id: SteppingAction.cc 67226 2013-02-08 12:07:18Z ihrivnac $
//
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4Step.hh"

#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "HistoManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(DetectorConstruction* det,
                                         EventAction* evt)
: G4UserSteppingAction(), 
  fDetector(det), fEventAction(evt)                                         
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
  // get volume of the current step
  G4VPhysicalVolume* volume 
  = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  
  // collect energy and track length step by step
  G4double edep = aStep->GetTotalEnergyDeposit();
  
  G4double stepl = 0.;
  if (aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.)
    stepl = aStep->GetStepLength();
      
  if (volume == fDetector->GetAbsorber()) fEventAction->AddAbs(edep,stepl);
  if (volume == fDetector->GetGap())      fEventAction->AddGap(edep,stepl);

  G4double intl = 0.10*cm;
  G4double bin_width = 0.03*cm;
  if (edep>0.) {

      G4ThreeVector prePoint  = aStep->GetPreStepPoint()->GetPosition();
      G4ThreeVector delta = aStep->GetPostStepPoint()->GetPosition() - prePoint;
      prePoint += G4UniformRand()*delta;
      
      G4double Rad = prePoint.perp();
      G4int N_int = G4int(Rad/(bin_width));

      if((prePoint.z() + 0.5*cm) < 1*intl && (prePoint.z() + 0.5*cm) > 0*intl){

	G4AnalysisManager::Instance()->FillP1(0, N_int, edep/MeV);
      }

      if((prePoint.z() + 0.5*cm) < 5*intl && (prePoint.z() + 0.5*cm) > 4*intl){

	G4AnalysisManager::Instance()->FillP1(1, N_int, edep/MeV);
      }
       if((prePoint.z() + 0.5*cm) < 10*intl && (prePoint.z() + 0.5*cm) > 9*intl){

	G4AnalysisManager::Instance()->FillP1(2, N_int, edep/MeV);
      }
       G4double perp = prePoint.z();
       G4AnalysisManager::Instance()->FillP2(0, perp, Rad, edep/MeV);
       G4AnalysisManager::Instance()->FillP2(0, perp, -Rad, edep/MeV);

  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
