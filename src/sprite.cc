//      (_||_/
//      (    )
//     ( o  0 )
//-OOO°--(_)---°OOO---------------------------------------
//                   Copyright (C) 2006 By Dominique Roux-Serret
// .OOOo      oOOO.  roux-serret@ifrance.com
//-(   )------(   )---------------------------------------
//  ( (        ) /   Copyright (C) 2023 By Johnny Jazeix
//   (_)      (_/    jazeix@gmail.com

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

#include <SDL2/SDL_log.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <cstring>
#include "sprite.h"
#include "preference.h"
#include "utils.h"

/*** Variables Globales ***/
/**************************/
extern SDL_Renderer *sdlRenderer;
extern Sprite *Sprites;
extern int NSprites;
extern sNewPreference Pref;

static const char *OrdreTexte = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-!?*+<>%$()&;";
static const char *OrdreTexte2 = "abcdefghijklmnopqrstuvwxyz0123456789,_|?*+<>%$[]&;";
static int TableTexte[256];

char Langue[31][16]; // Mémorise les noms des langues
int NTextes = 0;
bool shouldDrawLoading = false;

#define N_SPRITESFOND 2

/*** Affiche le chargeur lors du chargement ***/
/**********************************************/
void DrawLoading()
{
    static int NumAf = -1; // Numéro su sprite affiché
    int i, Old;
    int Clock;

    if (shouldDrawLoading == true) {
        Clock = SDL_GetTicks();
        i = (Clock / (1000 / 25)) % Sprites[loading].N; // Calcule le numéro su sprite à afficher

        if (i != NumAf) {
            Old = NumAf;
            NumAf = i;
            Sprites[loading].Draw(400, 300, NumAf);
            SDL_RenderPresent(sdlRenderer);
            // TODO if(Old!=-1) Sprites[chargeur].Efface(400,300,Old,sdlVideo);
        }
    }
}

/*** Charge les Sprites d'une langue ***/
/***************************************/
bool LoadLanguage()
{
    long L, P;
    int i;
    unsigned char *Buf;
    char PathFile[512];

    strcpy(PathFile, Langue[Pref.Language]);
    Utils::GetPath(PathFile);
    if (Utils::FileExists(PathFile) == false) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to find '%s'", Langue[Pref.Language]);
        return false;
    }
    L = Utils::LoadFile(PathFile, Buf);

    // Lit les sprites
    P = 0;
    for (i = 0; i < NTextes; i++) {
        if (Sprites[T_level + i].N) {
            Sprites[T_level + i].Delete();
        }
        if (Sprites[T_level + i].Load(Buf, P) == false) {
            return false;
        }
    }
    delete[] Buf; // Libère la mémoire du fichier des sprites

    return true;
}

/*** Charge les Sprites du jeu ***/
/*********************************/
bool LoadSprites()
{
    long L, P;
    int i;
    unsigned char *Buf;
    int NSp;

    char PathFile[512] = "language.dat";
    Utils::GetPath(PathFile);

    // Initialise la table de caractaire des textes
    for (i = 0; i < 256; i++) {
        TableTexte[i] = -1;
    }
    i = 0;
    while (OrdreTexte[i] != 0) {
        TableTexte[(int)(OrdreTexte[i])] = i;
        i++;
    }
    i = 0;
    while (OrdreTexte2[i] != 0) {
        TableTexte[(int)(OrdreTexte2[i])] = i;
        i++;
    }

    // *** Charge le fichier des langues ***
    // *************************************
    if (Utils::FileExists(PathFile) == false) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to find 'language.dat'");
        return false;
    }
    L = Utils::LoadFile(PathFile, Buf);

    // Prend le nombre de sprites
    NSp = (int)(Buf[0]) * 256 + (int)(Buf[1]);
    NSp += N_SPRITESFOND + 2;
    NTextes = (int)(Buf[2]) * 256 + (int)(Buf[3]);
    Pref.NLanguages = (int)(Buf[4]) * 256 + (int)(Buf[5]);

    NSprites = NSp + NTextes + Pref.NLanguages;
    Sprites = new Sprite[NSprites];

    // Récupère les nom des langues
    P = 6;
    for (i = 0; i < Pref.NLanguages; i++) {
        strcpy(Langue[i], (char *)(Buf + P));
        P += strlen((char *)(Buf + P)) + 1;
    }

    // Charge les sprites des langues
    for (i = 0; i < Pref.NLanguages; i++) {
        if (Sprites[T_Langue + i].Load(Buf, P) == false) {
            return false;
        }
    }

    if (Sprites[loading].Load(Buf, P) == false) {
        return false; // Sprite du chargeur
    }
    shouldDrawLoading = true; // Peut afficher le sprite du chargeur

    delete[] Buf;

    // *** Charge le fichier des sprites ***
    // *************************************
    strcpy(PathFile, "sprites.dat");
    Utils::GetPath(PathFile);
    if (Utils::FileExists(PathFile) == false) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to find 'sprites.dat'");
        return false;
    }
    L = Utils::LoadFile(PathFile, Buf);

    // Lit les sprites
    P = 0;
    for (i = 0; i < NSp; i++) {
        DrawLoading();
        switch (i) {
        case fjeu:
        case fmenu:
            if (Sprites[i].New(800, 600) == false) {
                return false;
            }
            break;
        case rope:
            Sprites[i].N = 0;
            break;
        case loading:
            break;
        default:
            if (Sprites[i].Load(Buf, P) == false) {
                return false;
            }
        }
    }

    delete[] Buf; // Libère la mémoire du fichier des sprites

    // *** Charge la langue ***
    // ************************
    if (Pref.Language != -1) {
        LoadLanguage();
    }

    shouldDrawLoading = false; // N'affiche plus les sprites du chargeur
    return true;
}

/*** Retourne la longueur d'un nombre ***/
/****************************************/
int NumberLength(int C)
{
    int l = 0;

    do {
        l += Sprites[digits].Dim[(C % 10)].L;
        C /= 10;
        if (C) {
            l += ECART_ENTRE_CHIFFRE;
        }
    } while (C);

    return l;
}

/*** Retourne la longueur d'un texte ***/
/***************************************/
int StringLength(char *Texte)
{
    int i = 0;
    int l = 0;
    int Le;

    while (Texte[i] != 0) {
        Le = (int)(Texte[i]);
        if (TableTexte[Le] != -1) {
            l += Sprites[lettres].Dim[(TableTexte[Le])].L;
            if (Texte[i + 1] != 0) {
                l += ECART_ENTRE_LETTRE;
            }
        }
        else {
            if (Le == (int)(' ')) {
                l += LONGUEUR_ESPACE;
            }
        }

        i++;
    }

    return l;
}

/*** Test si un caracataire existe ***/
/*************************************/
bool CharExist(char C)
{
    if ((int)(C) < 0) {
        return false;
    }
    if (C == ' ') {
        return true;
    }
    if (TableTexte[(int)(C)] != -1) {
        return true;
    }
    return false;
}
/*** Affiche un nombre ***/
/*************************/
void DrawNumber(int x, int y, int Nombre, SDL_Texture *Fond)
{
    int const l = NumberLength(Nombre);

    x += l / 2;
    do {
        Sprites[digits].Draw(x - (Sprites[digits].Dim[(Nombre % 10)].L) / 2, y, Nombre % 10, Fond);
        x -= Sprites[digits].Dim[(Nombre % 10)].L + ECART_ENTRE_CHIFFRE;
        Nombre /= 10;
    } while (Nombre);
}

/*** Affiche un Texte ***/
/************************/
void DrawString(int x, int y, char *Texte, SDL_Texture *Fond)
{
    int i = 0;
    int Le;

    // TODO Handle here unicode
    while (Texte[i] != 0) {
        Le = (int)(Texte[i]);

        if (TableTexte[Le] != -1) { // Si un caractaire connue
            Le = TableTexte[Le];
            Sprites[lettres].Draw(x + (Sprites[lettres].Dim[Le].L / 2), y, Le, Fond);
            x += Sprites[lettres].Dim[Le].L + ECART_ENTRE_LETTRE;
        }
        else { // Si un espace
            if (Le == (int)(' ')) {
                x += LONGUEUR_ESPACE - ECART_ENTRE_LETTRE;
            }
        }

        i++;
    }
}

/*** Affiche un text dans la langue ***/
/**************************************/
void DrawText(int x, int y, e_Sprite Text, SDL_Texture *Fond)
{
    Sprites[Text].Draw(x, y, 0, Fond);
}

/*** Constructeur ***/
/********************/
Sprite::~Sprite()
{
    if (N) {
        for (int i = 0; i < N; i++) {
            SDL_DestroyTexture(Image[i]);
        }
        delete[] Image;
        delete[] Dim;
    }
}

/*** Charge les sprites ***/
/**************************/
bool Sprite::Load(unsigned char *Buf, long &P)
{
    int i, j;
    unsigned char *B;
    unsigned long ul = 1; // test le type de processeur
    unsigned char *pul = (unsigned char *)(&ul);

    // Prend nombre de sprites
    N = (int)(Buf[P]) * 256 + (int)(Buf[P + 1]);
    P += 2;
    Dim = new s_Dim[N];
    Image = new SDL_Texture *[N];

    // Lit tous les sprites
    for (i = 0; i < N; i++) {
        // Affiche l'animation de chargement
        DrawLoading();

        // Lit les dimensions
        Dim[i].L = (int)(Buf[P]) * 256 + (int)(Buf[P + 1]);
        P += 2;
        Dim[i].H = (int)(Buf[P]) * 256 + (int)(Buf[P + 1]);
        P += 2;
        Dim[i].cx = (int)(Buf[P]) * 256 + (int)(Buf[P + 1]);
        P += 2;
        Dim[i].cy = (int)(Buf[P]) * 256 + (int)(Buf[P + 1]);
        P += 2;
        Dim[i].bpp = (int)(Buf[P]) * 256 + (int)(Buf[P + 1]);
        P += 2;

        // Fabrique la surface
        SDL_Surface *surface = SDL_CreateRGBSurface(0, Dim[i].L, Dim[i].H, Dim[i].bpp * 8,
                                                    0xff, 0xff00, 0xff0000, 0xff000000 * (Dim[i].bpp - 3));
        if (surface == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create the SDL Surface");
            return false;
        }

        // Copie les pixels
        SDL_LockSurface(surface);
        B = (unsigned char *)surface->pixels;

        if (pul[0] == 0) { // Processeur type Power PC, 68000, ..
            for (j = 0; j < Dim[i].L * Dim[i].H * Dim[i].bpp; j += Dim[i].bpp) {
                if (Dim[i].bpp == 4) {
                    B[j + 3] = Buf[P++];
                }
                B[j + 2] = Buf[P++];
                B[j + 1] = Buf[P++];
                B[j] = Buf[P++];
            }
        }
        else {
            for (j = 0; j < Dim[i].L * Dim[i].H * Dim[i].bpp; j++) {
                B[j] = Buf[P++];
            }
        }

        SDL_UnlockSurface(surface);
        Image[i] = SDL_CreateTextureFromSurface(sdlRenderer, surface);
        SDL_FreeSurface(surface);
    }

    return true;
}

/*** Affiche le sprite ***/
/*************************/
void Sprite::Draw(int X, int Y, int NumSpr, SDL_Texture *Fond) const
{
    SDL_Rect Position;
    SDL_Rect Di;

    Position.x = X - Dim[NumSpr].cx;
    Position.y = Y - Dim[NumSpr].cy;

    Position.w = Position.h = Di.x = Di.y = 0;
    Di.w = Dim[NumSpr].L;
    Di.h = Dim[NumSpr].H;
    Position.w = Di.w;
    Position.h = Di.h;
    // TODO if(Fond==NULL) Fond=sdlRenderer;

    SDL_RenderCopy(sdlRenderer, Image[NumSpr], nullptr, &Position);
}

/*** Print the white rope between two wagons ***/
/*********************************/
void Sprite::PrintRope(int dx, int dy, int fx, int fy)
{
    SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 0);
    SDL_RenderDrawLine(sdlRenderer, dx, dy, fx, fy);
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 0);
}

/*** Alloue un nouveau sprite vide ***/
/*************************************/
bool Sprite::New(int Lx, int Ly)
{
    Delete(); // Efface au cas ou

    N = 1;
    Dim = new s_Dim[N];
    Image = new SDL_Texture *[N];

    Dim[0].L = Lx;
    Dim[0].H = Ly;
    Dim[0].cx = Lx / 2;
    Dim[0].cy = Ly / 2;
    Dim[0].bpp = 3; // No transparency

    // Fabrique la surface
    SDL_Surface *surface = SDL_CreateRGBSurface(0, Dim[0].L, Dim[0].H, Dim[0].bpp * 8,
                                                0xff, 0xff00, 0xff0000, 0xff000000 * (Dim[0].bpp - 3));
    if (surface == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create the SDL Surface");
        return false;
    }
    Image[0] = SDL_CreateTextureFromSurface(sdlRenderer, surface);
    SDL_FreeSurface(surface);
    return true;
}

/*** Efface le sprite ***/
/************************/
void Sprite::Delete()
{
    if (N) {
        for (int i = 0; i < N; i++) {
            SDL_DestroyTexture(Image[i]);
        }
        delete[] Image;
        delete[] Dim;
        N = 0;
    }
}
