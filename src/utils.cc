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

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WINDOWS
#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#endif

#include "utils.h"
#include "preference.h"
#include "sprite.h"

/*** Variables globales ***/
/**************************/
extern sPreference Pref;
#ifdef LINUX
extern char DefPath[]; // Chemin par defaut dans arg
#endif

/*** Définition générals ***/
/***************************/
#ifdef MAC_OSX
#define MAC_LINUX
#endif

#ifdef LINUX
#define MAC_LINUX
#endif

/*** Test si un fichier exite ***/
/********************************/
bool FileExiste(const char *Path)
{
  FILE* file=fopen(Path,"r");

  if(file==NULL)
    return false;

  fclose(file);
  return true;
}


/*** Charge un fichier en Mémoire ***/
/************************************/
#ifdef MAC_LINUX
// Version linux
long ChargeFichier(const char *Path,unsigned char *&Buf)
{
  FILE *file;

  file=fopen(Path,"r");
  if(!file) {
    cerr <<"ERREUR: Impossible d'ouvrir '"<<Path<<"'"<<endl;
    perror("fopen");
    return -1;
  }
  
  if(fseek(file,0,2)!=0) {
    perror("fseek");
    fclose(file);
    return -1;
  }

  long L=ftell(file); // récupère la longueur
  fseek(file,0,0);

  Buf=new unsigned char [L+1];
  if(Buf==NULL) {
    cerr <<"ERREUR: Memoire insuffisante!"<<endl;
    fclose(file);
    return -1;
  }

  long Compt=L;
  unsigned char *Po=Buf;

  while(Compt>1024) {
    AfficheChargeur();
    if( fread(Po,1,1024,file) != 1024 ) {
      cerr <<"ERREUR de lecture du fichier '"<<Path<<"'"<<endl;
      perror("fread");
      fclose(file);
      delete [] Buf;
      return -1;
    }
    Compt-=1024;
    Po+=1024;
  }
  
  if(Compt) { // Ne fait pas le test à cause d'un bug dans windows
    fread(Po,1,(unsigned int)Compt,file);
  }
  
  fclose(file);
  return L;
}
#endif

#ifdef WINDOWS
// Version windows
long ChargeFichier(const char *Path,unsigned char *&Buf)
{
  HFILE file;

  file=_lopen(Path,OF_READ);
  if(file==-1) {
    cerr <<"Impossible d'ouvrir "<<Path<<endl;
    exit(-1);
  }
  
  long L=(long)_llseek(file,0,SEEK_END);
  if(L==-1) {
    cerr <<"Impossible de trouver la longueur du fichier"<<endl;
    perror("lseek");
    _lclose(file);
    return -1;
  }
  _llseek(file,0,SEEK_SET);
  
  Buf=new unsigned char [L+1];
  if(Buf==NULL) {
    cerr <<"ERREUR: Memoire insuffisante!"<<endl;
    _lclose(file);
    return -1;
  }

  AfficheChargeur();
  _hread(file,Buf,L); // Charge le fichier
  _lclose(file);      // Ferme le fichier

  return L;
}
#endif

/*** Sauve un Fichier ***/
/************************/
#ifdef MAC_LINUX
// Version linux
bool SauveFichier(const char *Path,char *Buf,long L)
{
  FILE *file;
  
  file=fopen(Path,"w");
  if(!file) {
    cerr <<"ERREUR: Impossible d'ouvrir '"<<Path<<"'"<<endl;
    perror("fopen");
    return false;
  }
  
  while(L>512) {
    if( fwrite(Buf,1,512,file) != 512 ) {
      cerr <<"ERREUR d'ecriture du fichier '"<<Path<<"'"<<endl;
      perror("fwrite");
      fclose(file);
      return false;
    }
    L-=512;
    Buf+=512;
  }

  if(L>0) {
    if( fwrite(Buf,1,(size_t)L,file) != (size_t)L ) {
      cerr <<"ERREUR d'ecriture du fichier '"<<Path<<"'"<<endl;
      perror("fwrite");
      fclose(file);
      return false;
    }
  }
  
  fclose(file);
  return true;
}
#endif

#ifdef WINDOWS
// Version windows
bool SauveFichier(const char *Path,char *Buf,long L)
{
  HFILE file;
  int Lec;

  file=_lcreat(Path,0);
  if(!file) {
    cerr <<"ERREUR: Impossible de créer le fichier '"<<Path<<"'"<<endl;
    return false;
  }
  
  Lec=_hwrite(file,Buf,L);
  _lclose(file);

  if(Lec!=L) {
    cerr <<"Problème d'ecriture du fichier '"<<Path<<"' ecris="<<Lec<<" au lieux de ="<<L<<endl;
    return false;
  }

  return true;
}
#endif

/*** Met le bon chemin pour charger un fichier ***/
/*************************************************/
#if defined(LINUX) && !defined(__AMIGAOS4__)
// Version Linux
void GetPath(char *Path)
{
  char Provi[512];

  strcpy(Provi,Path);

  if(DefPath[0]) {
    sprintf(Path,"%s%s",DefPath,Provi);
    if(FileExiste(Path)) return;
  }

  sprintf(Path,"%s/%s",DATA_DIR,Provi);
  if(FileExiste(Path)) return;
  sprintf(Path,"./%s",Provi);
  if(FileExiste(Path)) return;
  sprintf(Path,"/usr/local/share/Ri-li/%s",Provi);
  if(FileExiste(Path)) return;
  sprintf(Path,"/usr/share/Ri-li/%s",Provi);
  if(FileExiste(Path)) return;
  sprintf(Path,"/usr/share/games/Ri-li/%s",Provi);
  if(FileExiste(Path)) return;
  
  cerr <<"Impossible de trouver le fichier '"<<Provi<<endl;
  exit(-1);
}
#endif

#ifdef __AMIGAOS4__
// Version AmigaOS4
void GetPath(char *Path)
{
  char Provi[512];

  strcpy(Provi,Path);

  sprintf(Path,"PROGDIR:%s",Provi);
  if(FileExiste(Path)) return;
  
  cerr <<"Impossible de trouver le fichier '"<<Path<<endl;
  exit(-1);
}
#endif

#ifdef MAC_OSX
// Version Mac OSX
void GetPath(char *Path)
{
  char Provi[512];

  strcpy(Provi,Path);

  sprintf(Path,"Ri-li.app/Contents/Resources/%s",Provi);
  if(FileExiste(Path)) return;
  
  cerr <<"Impossible de trouver le fichier '"<<Path<<endl;
  exit(-1);
}
#endif

#ifdef WINDOWS
//  Version Windows , chemin directe
void GetPath(char *Path)
{ }
#endif

/*** Charge les préferences ***/
/******************************/
bool LoadPref(void)
{
  int L;
  unsigned char *Provi;
  
#if defined(LINUX) && !defined(__AMIGAOS4__)
  char PathPref[512];
  char *Env=getenv("HOME");
  sprintf(PathPref,"%s/.ri-li.pref",Env);
#elif defined(__AMIGAOS4__)
  char PathPref[512];
  char *Env="PROGDIR:";
  sprintf(PathPref,"%s.ri-li.pref",Env);
#endif

#ifdef MAC_OSX
  char PathPref[512];
  char *Env=getenv("HOME");
  sprintf(PathPref,"%s/Library/Preferences/Ri-li.pref",Env);
#endif

#ifdef WINDOWS
  // Version Windows, Chemin directe
  char *PathPref="ri-li.pref";
#endif
  
  if(FileExiste(PathPref)) {
    L=ChargeFichier(PathPref,Provi);
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
void SauvePref(void)
{  
#if defined(LINUX) && !defined(__AMIGAOS4__)
  char Provi[512];
  char *Env=getenv("HOME");
  sprintf(Provi,"%s/.ri-li.pref",Env);
  SauveFichier(Provi,(char*)&Pref,sizeof(sPreference));
#elif defined(__AMIGAOS4__)
  char Provi[512];
  char *Env="PROGDIR:";
  sprintf(Provi,"%s.ri-li.pref",Env);
  SauveFichier(Provi,(char*)&Pref,sizeof(sPreference));
#endif

#ifdef MAC_OSX
  char Provi[512];
  char *Env=getenv("HOME");
  sprintf(Provi,"%s/Library/Preferences/Ri-li.pref",Env);
  SauveFichier(Provi,(char*)&Pref,sizeof(sPreference));
#endif

#ifdef WINDOWS
  // Version Windows, chemin directe
  SauveFichier("ri-li.pref",(char*)&Pref,sizeof(sPreference));
#endif
}

