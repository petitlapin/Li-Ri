//      (_||_/
//      (    )       Classe Sprite
//     ( o  0 )
//-OOO°--(_)---°OOO---------------------------------------
//                   Copyright (C) 2006 By Dominique Roux-Serret
// .OOOo      oOOO.  roux-serret@ifrance.com
//-(   )------(   )---------------------------------------
//  ( (        ) /   Le 11/01/2006
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
#include <math.h>
#include "sprite.h"
#include "preference.h"
#include "utils.h"

/*** Variables Globales ***/
/**************************/
extern SDL_Surface *sdlVideo;
extern SDL_VideoInfo *sdlVideoInfo;
extern Uint32 FontColor;
extern Sprite *Sprites;
extern int NSprites;
extern sPreference Pref;

static char* OrdreTexte="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-!?*+<>%$()&;";
static char* OrdreTexte2="abcdefghijklmnopqrstuvwxyz0123456789,_|?*+<>%$[]&;";
static int TableTexte[256]; 

char Langue[31][16]; // Mémorise les noms des langues
int NTextes=0;
bool AfficheC=false; // Si peut afficher le chargeur lors du chargement

#define N_SPRITESFOND 2

/*** Affiche le chargeur lors du chargement ***/
/**********************************************/
void AfficheChargeur()
{
  static int NumAf=-1; // Numéro su sprite affiché
  int i,Old;
  int Hor;

  if(AfficheC==true) {
    Hor=SDL_GetTicks();
    i=(Hor/(1000/25))%Sprites[chargeur].N; // Calcule le numéro su sprite à afficher

    if(i!=NumAf) {
      Old=NumAf;
      NumAf=i;
      Sprites[chargeur].Affiche(400,300,NumAf);
      SDL_Flip(sdlVideo);
      if(Old!=-1) Sprites[chargeur].Efface(400,300,Old,sdlVideo);
    }
  }
  
}

/*** Charge les Sprites d'une langue ***/
/***************************************/
bool LoadLangue()
{
  long L,P;
  int i;
  unsigned char *Buf;
  char PathFile[512];

  strcpy(PathFile,Langue[Pref.Langue]);
  GetPath(PathFile);
  if(FileExiste(PathFile)==false) {
    cerr <<"Impossible de trouver "<<Langue[Pref.Langue]<<endl;
    return false;
  }
  L=ChargeFichier(PathFile,Buf);

  // Lit les sprites
  P=0;
  for(i=0;i<NTextes;i++) {
    if(Sprites[T_level+i].N) Sprites[T_level+i].Delete();
    if(Sprites[T_level+i].Load(Buf,P)==false) return false;
  }
  delete [] Buf; // Libère la mémoire du fichier des sprites

  return true;
}

/*** Charge les Sprites du jeu ***/
/*********************************/
bool LoadSprites()
{
  long L,P;
  int i;
  unsigned char *Buf;
  int NSp;

  char PathFile[512]="language.dat";
  GetPath(PathFile);

  // Initialise la table de caractaire des textes
  for(i=0;i<256;i++) TableTexte[i]=-1;
  i=0;
  while(OrdreTexte[i]!=0) {
    TableTexte[(int)(OrdreTexte[i])]=i;
    i++;
  }
  i=0;
  while(OrdreTexte2[i]!=0) {
    TableTexte[(int)(OrdreTexte2[i])]=i;
    i++;
  }
  
  // *** Charge le fichier des langues ***
  // *************************************
  if(FileExiste(PathFile)==false) {
    cerr <<"Impossible de trouver 'language.dat'"<<endl;
    return false;
  }
  L=ChargeFichier(PathFile,Buf);

  // Prend le nombre de sprites
  NSp=(int)(Buf[0])*256+(int)(Buf[1]);
  NSp+=N_SPRITESFOND+2;
  NTextes=(int)(Buf[2])*256+(int)(Buf[3]);
  Pref.NLangues=(int)(Buf[4])*256+(int)(Buf[5]);
  
  NSprites=NSp+NTextes+Pref.NLangues;
  Sprites=new Sprite[NSprites];

  // Récupère les nom des langues
  P=6;
  for(i=0;i<Pref.NLangues;i++) {
    strcpy(Langue[i],(char*)(Buf+P));
    P+=strlen((char*)(Buf+P))+1;
  }

  // Charge les sprites des langues
  for(i=0;i<Pref.NLangues;i++)
    if(Sprites[T_Langue+i].Load(Buf,P)==false) return false;
  
  if(Sprites[chargeur].Load(Buf,P)==false) return false; // Sprite du chargeur
  AfficheC=true; // Peut afficher le sprite du chargeur

  delete [] Buf;
  
  // *** Charge le fichier des sprites ***
  // *************************************
  strcpy(PathFile,"sprites.dat");
  GetPath(PathFile);
  if(FileExiste(PathFile)==false) {
    cerr <<"Impossible de trouver 'sprites.dat'"<<endl;
    return false;
  }
  L=ChargeFichier(PathFile,Buf);
  
  // Lit les sprites
  P=0;
  for(i=0;i<NSp;i++) {
    AfficheChargeur();
    switch(i) {
    case fjeu:
    case fmenu:
      if(Sprites[i].Nouveau(800,600)==false) return false;
      break;
    case corde:
      Sprites[i].N=0;
      break;
    case chargeur:
      break;
    default:
      if(Sprites[i].Load(Buf,P)==false) return false;
    }
  }
  
  delete [] Buf; // Libère la mémoire du fichier des sprites
  
  // *** Charge la langue ***
  // ************************
  if(Pref.Langue!=-1) LoadLangue();

  AfficheC=false; // N'affiche plus les sprites du chargeur
  return true;
}

/*** Retourne la longueur d'un nombre ***/
/****************************************/
int LongueurChiffre(int C)
{
  int l=0;
  
  do {
    l+=Sprites[chiffres].Dim[(C%10)].L;
    C/=10;
    if(C) l+=ECART_ENTRE_CHIFFRE;
  } while(C);
  
  return l;
}

/*** Retourne la longueur d'un texte ***/
/***************************************/
int LongueurString(char *Texte)
{
  int i=0;
  int l=0;
  int Le;

  while(Texte[i]!=0) {
    Le=(int)(Texte[i]);
    if(TableTexte[Le]!=-1) {
      l+=Sprites[lettres].Dim[(TableTexte[Le])].L;
      if(Texte[i+1]!=0) l+=ECART_ENTRE_LETTRE;
    }
    else {
      if(Le==(int)(' ')) l+=LONGUEUR_ESPACE;
    }
    
    i++;
  }
  
  return l;
}

/*** Test si un caracataire existe ***/
/*************************************/
bool CharExiste(char C)
{
  if(C==' ') return true;
  if(TableTexte[(int)(C)]!=-1) return true;
  return false;
}
/*** Affiche un nombre ***/
/*************************/
void AfficheChiffre(int x,int y,int Nombre,SDL_Surface *Fond)
{
  int l=LongueurChiffre(Nombre);
  
  x+=l/2;
  do {
    Sprites[chiffres].Affiche(x-(Sprites[chiffres].Dim[(Nombre%10)].L)/2,y,Nombre%10,Fond);
    x-=Sprites[chiffres].Dim[(Nombre%10)].L+ECART_ENTRE_CHIFFRE;
    Nombre/=10;
  } while(Nombre);
  
}

/*** Affiche un Texte ***/
/************************/
void AfficheString(int x,int y,char *Texte,SDL_Surface *Fond)
{
  int i=0;
  int Le;
  
  while(Texte[i]!=0) {
    Le=(int)(Texte[i]);
    
    if(TableTexte[Le]!=-1) { // Si un caractaire connue
      Le=TableTexte[Le];
      Sprites[lettres].Affiche(x+(Sprites[lettres].Dim[Le].L/2),y,Le,Fond);
      x+=Sprites[lettres].Dim[Le].L+ECART_ENTRE_LETTRE;
    }
    else { // Si un espace
      if(Le==(int)(' ')) x+=LONGUEUR_ESPACE-ECART_ENTRE_LETTRE;
    }
    
    i++;
  }
}

/*** Efface un nombre ***/
/************************/
void EffaceChiffre(int x,int y,int Nombre,SDL_Surface *Fond)
{
  int l=LongueurChiffre(Nombre);
  int h=Sprites[chiffres].Dim[0].H;

  if(!Fond) Fond=Sprites[fjeu].Image[0];
  Sprites[chiffres].EffaceCarre(x-l/2,y-h/2,x+l/2,y+h/2,Fond);
}

/*** Affiche un text dans la langue ***/
/**************************************/
void AfficheText(int x,int y,e_Sprite Text,SDL_Surface *Fond)
{
  Sprites[Text].Affiche(x,y,0,Fond);
}

/*** Constructeur ***/
/********************/
Sprite::Sprite(void) : N(0), Image(NULL), Dim(NULL)
{ }

Sprite::~Sprite(void)
{
  if(N) {
    for(int i=0;i<N;i++) {
      SDL_FreeSurface(Image[i]);
    }
    delete [] Image;
    delete [] Dim;
  }
}

/*** Charge les sprites ***/
/**************************/
bool Sprite::Load(unsigned char *Buf,long &P)
{
  int i,j;
  unsigned char *B;
  unsigned long ul=1; // test le type de processeur
  unsigned char *pul=(unsigned char*)(&ul) ;

  // Prend nombre de sprites
  N=(int)(Buf[P])*256+(int)(Buf[P+1]);
  P+=2;
  Dim=new s_Dim[N];
  Image=new SDL_Surface* [N];

  // Lit tous les sprites
  for(i=0;i<N;i++) {
    // Affiche l'animation de chargement
    AfficheChargeur();

    // Lit les dimensions
    Dim[i].L=(int)(Buf[P])*256+(int)(Buf[P+1]);
    P+=2;
    Dim[i].H=(int)(Buf[P])*256+(int)(Buf[P+1]);
    P+=2;
    Dim[i].cx=(int)(Buf[P])*256+(int)(Buf[P+1]);
    P+=2;
    Dim[i].cy=(int)(Buf[P])*256+(int)(Buf[P+1]);
    P+=2;
    Dim[i].bpp=(int)(Buf[P])*256+(int)(Buf[P+1]);
    P+=2;
    
    // Fabrique la surface
    Image[i]=SDL_CreateRGBSurface((Dim[i].bpp-3)*SDL_SRCALPHA,Dim[i].L,Dim[i].H,Dim[i].bpp*8,
				  0xff,0xff00,0xff0000,0xff000000*(Dim[i].bpp-3));
    if(Image[i]<=NULL) {
      cerr <<"Impossible de créer une Surface SDL!"<<endl;
      return false;
    }
    
    // Copie les pixels
    SDL_LockSurface(Image[i]);
    B=(unsigned char*)Image[i]->pixels;
    
    if(pul[0]==0) { // Processeur type Power PC, 68000, ..
      for(j=0;j<Dim[i].L*Dim[i].H*Dim[i].bpp;j+=Dim[i].bpp) {
	if(Dim[i].bpp==4) B[j+3]=Buf[P++];
	B[j+2]=Buf[P++];
	B[j+1]=Buf[P++];
	B[j]=Buf[P++];
      }
    }
    else
      for(j=0;j<Dim[i].L*Dim[i].H*Dim[i].bpp;j++) B[j]=Buf[P++];
    
    SDL_UnlockSurface(Image[i]);
    
    // Si peut la mettre en surface Hardware
    if(sdlVideoInfo->blit_hw_A) {
      SDL_Surface *Provi=SDL_ConvertSurface(Image[i],Image[i]->format,SDL_HWSURFACE);
      SDL_FreeSurface(Image[i]);
      Image[i]=Provi;
    }
  }
  
  return true;
}

/*** Affiche le sprite ***/
/*************************/
void Sprite::Affiche(int X,int Y,int NumSpr,SDL_Surface *Fond)
{
  SDL_Rect Position;
  SDL_Rect Di;

  Position.x=X-Dim[NumSpr].cx;
  Position.y=Y-Dim[NumSpr].cy;
  
  Position.w=Position.h=Di.x=Di.y=0;
  Di.w=Dim[NumSpr].L;
  Di.h=Dim[NumSpr].H;
  
  if(Fond==NULL) Fond=sdlVideo;

  SDL_BlitSurface(Image[NumSpr],&Di,Fond,&Position);
}

/*** Affiche un bout du sprite ***/
/*********************************/
void Sprite::AfficheCorde(int dx,int dy,int fx,int fy)
{
  float d;
  float lx=float(fx-dx);
  float ly=float(fy-dy);
  int P,n,i,j;
  unsigned char *B;
  float x=dx;
  float y=dy;

  d=sqrt(lx*lx+ly*ly); // Longueur de la corde
  if( (float)((int)(d))!=d ) n=(int)(d)+1; // Met en nombre de point
  else n=(int)d;
  
  // Trace la ligne
  SDL_LockSurface(sdlVideo);
  B=(unsigned char*)sdlVideo->pixels;
  
  lx=lx/(float)n;
  ly=ly/(float)n;

  for(i=0;i<=n;i++) {
    P=((int)(x)+(int)(y)*800)*sdlVideo->format->BytesPerPixel;
    for(j=0;j<sdlVideo->format->BytesPerPixel;j++) B[P+j]=255;
    x+=lx;
    y+=ly;
  }

  SDL_UnlockSurface(sdlVideo);
}

/*** Efface le sprite ***/
/************************/
void Sprite::Efface(int X,int Y,int NumSpr,SDL_Surface *Fond)
{
  SDL_Rect Position;
  
  Position.w=Dim[NumSpr].L;
  Position.h=Dim[NumSpr].H;
  Position.x=X-Dim[NumSpr].cx;
  Position.y=Y-Dim[NumSpr].cy;
  SDL_BlitSurface(Fond,&Position,sdlVideo,&Position);
}

/*** Efface un carré à l'ecran ***/
/*********************************/
void Sprite::EffaceCarre(int dx,int dy,int fx,int fy,SDL_Surface *Fond)
{
  int p;
  SDL_Rect Position;
  
  if(dx>fx) { p=dx; dx=fx; fx=p; }
  if(dy>fy) { p=dy; dy=fy; fy=p; }

  Position.w=fx-dx+1;
  Position.h=fy-dy+1;
  Position.x=dx;
  Position.y=dy;
  SDL_BlitSurface(Fond,&Position,sdlVideo,&Position);
}

/*** Alloue un nouveau sprite vide ***/
/*************************************/
bool Sprite::Nouveau(int Lx,int Ly)
{
  Delete(); // Efface au cas ou

  N=1;
  Dim=new s_Dim[N];
  Image=new SDL_Surface* [N];

  Dim[0].L=Lx;
  Dim[0].H=Ly;
  Dim[0].cx=Lx/2;
  Dim[0].cy=Ly/2;
  Dim[0].bpp=3; // Pas de transparence
     
  // Fabrique la surface
  Image[0]=SDL_CreateRGBSurface((Dim[0].bpp-3)*SDL_SRCALPHA,Dim[0].L,Dim[0].H,Dim[0].bpp*8,
				0xff,0xff00,0xff0000,0xff000000*(Dim[0].bpp-3));
  if(Image[0]<=NULL) {
    cerr <<"Impossible de créer une Surface SDL!"<<endl;
    return false;
  }
  return true;
}

/*** Efface le sprite ***/
/************************/
void Sprite::Delete(void)
{
  if(N) {
    for(int i=0;i<N;i++) {
      SDL_FreeSurface(Image[i]);
    }
    delete [] Image;
    delete [] Dim;
    N=0;
  }
}
