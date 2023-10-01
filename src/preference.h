//      (_||_/
//      (    )       Structure des Preferences
//     ( o  0 )
//-OOO°--(_)---°OOO---------------------------------------
//                   Copyright (C) 2006 By Dominique Roux-Serret
// .OOOo      oOOO.  roux-serret@ifrance.com
//-(   )------(   )---------------------------------------
//  ( (        ) /   Le 14/01/2006
//   (_)      (_/

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 or version 3 of the License.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#ifndef _PREFERENCE_DOM_
#define _PREFERENCE_DOM_

/*** Défintitions ***/
#define VITESSE_MAX 180.0
#define VITESSE_MOY 120.0
#define VITESSE_MIN 80.0
#define DUREE_VITESSE (10*1000)

#define DUREE_PAUSE (3*1000)

#define DUREE_REDUIT (10*1000)
#define DUREE_ALONGE (10*1000)
#define ECARTWAGON_MAX 27.0
#define ECARTWAGON_MOY 13.0
#define ECARTWAGON_MIN 4.0

#define N_VIES_DEP 3

#define RAYON_TOUCHE (32*32)

#define LT 17
#define HT 15
#define MAX_N_LEVEL_IN_MEMORY 256

#define D_Gauche 0
#define D_Droite 1
#define D_Haut 2
#define D_Bas 3

#define D_Case 40
#define D_CaseR 31.416

/*** Pieces Posibles sur le Tableau ***/
#define C_None 0
#define C_Rail 1
#define C_Wagon 2
#define C_Allonge 3
#define C_Reduit 4
#define C_Speed 5
#define C_Live 6
#define C_Fin 7

/*** Difficultées possible ***/
enum e_Difficulte {
  Easy,
  Normal,
  Hard
};

/*** Mémorise un score ***/
#define LSCOREMAX 460

struct sScore {
  int Score;
  char Name[80];
};

/*** Structure des preferences Générales ***/
struct sPreference {
  e_Difficulte Difficulte; // Difficulté de la partie (Vitesse)
  int Niveau;        // Niveau du joueur
  int NVie;          // Nombre de vie du joueur
  int Score;         // Score du joueur
  double Vitesse;    // Vitesse suivant le niveau
  double VitesseMoy; // Vitesse en cours de la loco
  float EcartWagon;  // ecart en pixels entre 2 wagons
  int NiveauMax;
  int FullScreen;
  int Langue;        // Langue à afficher
  int NLangues;      // Nombre de langues disponible
  float Volume;        // Volumes audio
  float VolumeM;       // Volume de la music
  struct sScore Sco[8]; // Mémorise les scores
};

/*** Valeur retournées pour le menu princiaple ***/
enum eMenu {
  mMenu,
  mMenuJeux,
  mMenuSpeed,
  mMenuNiveau,
  mLangue,
  mJeux,
  mScore,
  mScoreEdit,
  mOption,
  mQuit,
  mEdit,
};

#endif
