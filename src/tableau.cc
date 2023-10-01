//      (_||_/
//      (    )       Classe Sprite
//     ( o  0 )
//-OOO°--(_)---°OOO---------------------------------------
//                   Copyright (C) 2006 By Dominique Roux-Serret
// .OOOo      oOOO.  roux-serret@ifrance.com
//-(   )------(   )---------------------------------------
//  ( (        ) /   Le 3/04/2006
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
#include "preference.h"
#include "utils.h"
#include "tableau.h"

/*** Constructeurs ***/
/*********************/
Tableau::Tableau(void) : N(0)
{
  int i;
  for(i=0;i<MAX_N_LEVEL_IN_MEMORY;i++) Clear(i);
}

Tableau::~Tableau(void)
{
}

/*** Charge les tableaux ***/
/***************************/
bool Tableau::Load(void)
{
  unsigned char *Buf;
  long L,P=2;
  int i,j;
  char NameLevelFile[512]="levels.dat";

  GetPath(NameLevelFile);
  L=ChargeFichier(NameLevelFile,Buf);
  if(L<=0) return false;
  
  // Charge les tableaux
  N=(int)(Buf[0])*256+(int)(Buf[1]);
  
  for(i=0;i<N;i++) {
    for(j=0;j<LT*HT;j++) T[i].T[j]=Buf[P++];
    T[i].DepX=(int)(Buf[P])*256+(int)(Buf[P+1]);
    T[i].DepY=(int)(Buf[P+2])*256+(int)(Buf[P+3]);
    T[i].DepDir=(int)(Buf[P+4])*256+(int)(Buf[P+5]);
    T[i].NDeco=(int)(Buf[P+6])*256+(int)(Buf[P+7]);
    P+=8;
    for(j=0;j<T[i].NDeco;j++) {
      T[i].Deco[j].NumSpr=(int)(Buf[P])*256+(int)(Buf[P+1]);
      T[i].Deco[j].x=(int)(Buf[P+2])*256+(int)(Buf[P+3]);
      T[i].Deco[j].y=(int)(Buf[P+4])*256+(int)(Buf[P+5]);
      P+=6;
    }
  }

  delete [] Buf;
  
  return true;
}

/*** Sauve les tableaux ***/
/**************************/
bool Tableau::Save(void)
{
  unsigned char *Buf;
  long P=2;
  int i,j;
  char NameLevelFile[512]="levels.dat";
  
  // Alloue la mémoire
  Buf=new unsigned char [sizeof(s_Tableau)*N+sizeof(int)+1];
  if(Buf<=0) return false;

  // Charge les tableaux
  Buf[0]=N/256;
  Buf[1]=N%256;
  
  for(i=0;i<N;i++) {
    for(j=0;j<LT*HT;j++) Buf[P++]=T[i].T[j];
    Buf[P]=T[i].DepX/256;
    Buf[P+1]=T[i].DepX%256;
    Buf[P+2]=T[i].DepY/256;
    Buf[P+3]=T[i].DepY%256;
    Buf[P+4]=T[i].DepDir/256;
    Buf[P+5]=T[i].DepDir%256;
    Buf[P+6]=T[i].NDeco/256;
    Buf[P+7]=T[i].NDeco%256;

    P+=8;
    for(j=0;j<T[i].NDeco;j++) {
      Buf[P]=T[i].Deco[j].NumSpr/256;
      Buf[P+1]=T[i].Deco[j].NumSpr%256;
      Buf[P+2]=T[i].Deco[j].x/256;
      Buf[P+3]=T[i].Deco[j].x%256;
      Buf[P+4]=T[i].Deco[j].y/256;
      Buf[P+5]=T[i].Deco[j].y%256;
      P+=6;
    }
  }

  // Sauve les tableaux
  GetPath(NameLevelFile);
  if(SauveFichier(NameLevelFile,(char*)Buf,P)==false) {
    delete [] Buf;
    return false;
  }
  
  delete [] Buf;  
  return true;
}

/*** Efface un Tableau ***/
/*************************/
void Tableau::Del(int Num)
{
  int i,j;

  if(Num<N) {
    for(i=Num;i<N-1;i++) {
      for(j=0;j<LT*HT;j++) T[i].T[j]=T[i+1].T[j];
      T[i].DepX=T[i+1].DepX;
      T[i].DepY=T[i+1].DepY;
      T[i].DepDir=T[i+1].DepDir;
      T[i].NDeco=T[i+1].NDeco;
      for(j=0;j<T[i].NDeco;j++) {
	T[i].Deco[j].x=T[i+1].Deco[j].x;
	T[i].Deco[j].y=T[i+1].Deco[j].y;
	T[i].Deco[j].NumSpr=T[i+1].Deco[j].NumSpr;
      }
    }
    N--;
  }
}

/*** Insert un Tableau ***/
/*************************/
void Tableau::Ins(int Num)
{
  int i,j;
  
  if(Num<N) {
    for(i=N;i>Num;i--) {
      for(j=0;j<LT*HT;j++) T[i].T[j]=T[i-1].T[j];
      T[i].DepX=T[i-1].DepX;
      T[i].DepY=T[i-1].DepY;
      T[i].DepDir=T[i-1].DepDir;
      T[i].NDeco=T[i-1].NDeco;
      for(j=0;j<T[i].NDeco;j++) {
	T[i].Deco[j].x=T[i-1].Deco[j].x;
	T[i].Deco[j].y=T[i-1].Deco[j].y;
	T[i].Deco[j].NumSpr=T[i-1].Deco[j].NumSpr;
      }
    }

    Clear(Num);
    N++;
  }
}

/*** Vide un tableau ***/
/***********************/
void Tableau::Clear(int Num)
{
  int i;

  for(i=0;i<LT*HT;i++) T[Num].T[i]=C_None;
  T[Num].DepX=LT/2;
  T[Num].DepY=HT/2;
  T[Num].NDeco=0;
}
