//      (_||_/       Utils.h
//      (    )       Fonctions divers pour les differents systèmes d'exploitation
//     ( o  0 )
//-OOO°--(_)---°OOO---------------------------------------
//                   Copyright (C) 2006 By Dominique Roux-Serret
// .OOOo      oOOO.  roux-serret@ifrance.com
//-(   )------(   )---------------------------------------
//  ( (        ) /   Le 03/01/2006
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

#ifndef _UTILS_DOM_
#define _UTILS_DOM_

bool FileExiste(const char *Path);    // Si un fichier existe
long ChargeFichier(const char *Path,unsigned char *&Buf); // Charge un ficher en mémoire
bool SauveFichier(const char *Path,char *Buf,long L); // Sauve un Fichier

void GetPath(char *Name); // Rajoute le chemin au nom du fichier (sprites, levels)

bool LoadPref(void); // Charger les préferences
void SauvePref(void); // Sauve les preferences
#endif
