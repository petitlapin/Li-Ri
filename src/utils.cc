//      (_||_/       Utils.cc
//      (    )       Fonctions divers
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

#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "preference.h"
#include "sprite.h"

/*** Variables globales ***/
/**************************/
extern sPreference Pref;
#ifdef __unix__
extern char DefPath[]; // Chemin par defaut dans arg
#endif

/*** Définition générals ***/
/***************************/
#ifdef __APPLE__
#define MAC_LINUX
#endif

#ifdef __unix__
#define MAC_LINUX
#endif

#ifdef ANDROID
#define MAC_LINUX
#endif

/*** Test si un fichier exite ***/
/********************************/
bool Utils::FileExiste(const char *Path)
{
  SDL_RWops* file=SDL_RWFromFile(Path,"rb");

  if(file==NULL)
    return false;

  SDL_RWclose(file);
  return true;
}


/*** Charge un fichier en Mémoire ***/
/************************************/
long Utils::ChargeFichier(const char *Path,unsigned char *&Buf)
{
  SDL_RWops* file=SDL_RWFromFile(Path,"rb");
  if(!file) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to open '%s'", Path);
    return -1;
  }
  
  if(SDL_RWseek(file,0,RW_SEEK_END)<0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to compute file size");
    return -1;
  }

  long L=SDL_RWtell(file); // récupère la longueur
  SDL_RWseek(file,0,RW_SEEK_SET);

  Buf=new unsigned char [L+1];
  if(Buf==NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Not enough memory");
    SDL_RWclose(file);
    return -1;
  }

  long Compt=L;
  unsigned char *Po=Buf;

  while(Compt>1024) {
    AfficheChargeur();
    if( SDL_RWread(file,Po,1,1024) != 1024 ) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error while reading '%s'", Path);
      SDL_RWclose(file);
      delete [] Buf;
      return -1;
    }
    Compt-=1024;
    Po+=1024;
  }
  
  if(Compt) { // Ne fait pas le test à cause d'un bug dans windows
    SDL_RWread(file,Po,1,(unsigned int)Compt);
  }
  
  SDL_RWclose(file);
  return L;
}

/*** Sauve un Fichier ***/
/************************/
bool Utils::SauveFichier(const char *Path,char *Buf,long L)
{
  FILE *file;
  
  file=fopen(Path,"w");
  if(!file) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to open '%s'", Path);
    return false;
  }
  
  while(L>512) {
    if( fwrite(Buf,1,512,file) != 512 ) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to write in '%s'", Path);
      fclose(file);
      return false;
    }
    L-=512;
    Buf+=512;
  }

  if(L>0) {
    if( fwrite(Buf,1,(size_t)L,file) != (size_t)L ) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to write in '%s'", Path);
      fclose(file);
      return false;
    }
  }
  
  fclose(file);
  return true;
}

/*** Met le bon chemin pour charger un fichier ***/
/*************************************************/
#if (defined(__unix__) || defined(ANDROID)) && !defined(__AMIGAOS4__)
// Version Linux
void Utils::GetPath(char *Path)
{
  char Provi[512];

  strcpy(Provi,Path);

#ifndef ANDROID
  if(DefPath[0]) {
    sprintf(Path,"%s%s",DefPath,Provi);
    if(Utils::FileExiste(Path)) return;
  }
  sprintf(Path,"%s/%s",DATA_DIR,Provi);
  if(Utils::FileExiste(Path)) return;
#endif

  // Android is directly the filename
  sprintf(Path,"%s",Provi);
  if(Utils::FileExiste(Path)) return;

  sprintf(Path,"./%s",Provi);
  if(Utils::FileExiste(Path)) return;
  sprintf(Path,"/usr/local/share/Li-ri/%s",Provi);
  if(Utils::FileExiste(Path)) return;
  sprintf(Path,"/usr/share/Li-ri/%s",Provi);
  if(Utils::FileExiste(Path)) return;
  sprintf(Path,"/usr/share/games/Li-ri/%s",Provi);
  if(Utils::FileExiste(Path)) return;
  
  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to find '%s'", Provi);
}
#endif

#ifdef __AMIGAOS4__
// Version AmigaOS4
void Utils::GetPath(char *Path)
{
  char Provi[512];

  strcpy(Provi,Path);

  sprintf(Path,"PROGDIR:%s",Provi);
  if(Utils::FileExiste(Path)) return;
  
  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to find '%s'", Path);
  exit(-1);
}
#endif

#ifdef __APPLE__
// Version Mac OSX
void Utils::GetPath(char *Path)
{
  char Provi[512];

  strcpy(Provi,Path);

  sprintf(Path,"Li-ri.app/Contents/Resources/%s",Provi);
  if(Utils::FileExiste(Path)) return;
  
  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to find '%s'", Path);
  exit(-1);
}
#endif

#ifdef _WIN32
//  Version Windows , chemin directe
void Utils::GetPath(char *Path)
{
  char Provi[512];
   
  strcpy(Provi,Path);
  char *basePath = SDL_GetBasePath();
  sprintf(Path,"%s/%s",basePath,Provi);
  if(Utils::FileExiste(Path)) {
      SDL_free(basePath);
      return;
  }
  SDL_free(basePath);
}
#endif

/*** Charge les préferences ***/
/******************************/
bool Utils::LoadPref(void)
{
  int L;
  unsigned char *Provi;
  
  char PathPref[512];
  char *PrefFolder = SDL_GetPrefPath("Li-Ri", "Li-Ri");
  sprintf(PathPref, "%sli-ri.pref", PrefFolder);

  SDL_free(PrefFolder);

  if(Utils::FileExiste(PathPref)) {
    L=Utils::ChargeFichier(PathPref,Provi);
    if(L>0) {
      memcpy((char*)&Pref,Provi,L);
      delete [] Provi;
      return true;
    }
  }
  
  return false;
}

/*** Sauve les preferences ***/
/*****************************/
void Utils::SauvePref(void)
{  
  char PathPref[512];
  char *PrefFolder = SDL_GetPrefPath("Li-Ri", "Li-Ri");
  sprintf(PathPref, "%sli-ri.pref", PrefFolder);

  SDL_free(PrefFolder);
  Utils::SauveFichier(PathPref,(char*)&Pref,sizeof(sPreference));
}

