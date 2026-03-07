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

extern SDL_Renderer *sdlRenderer;
extern Sprite *Sprites;
extern int NSprites;
extern sNewPreference Pref;

static const char *TextOrder = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-!?*+<>%$()&;";
static const char *TextOrder2 = "abcdefghijklmnopqrstuvwxyz0123456789,_|?*+<>%$[]&;";
static int TextTable[256];

char Languages[31][16]; // Stores languages
int NTexts = 0;
bool shouldDrawLoading = false;

#define N_SPRITESFOND 2

/*** Displays loader when loading ***/
/************************************/
void DrawLoading()
{
    static int NumDisplayed = -1; // Number of the displayed sprite
    int i, Old;
    int Clock;

    if (shouldDrawLoading == true) {
        Clock = SDL_GetTicks();
        i = (Clock / (1000 / 25)) % Sprites[loading].N; // Calculates the Number of the wanted sprite

        if (i != NumDisplayed) {
            Old = NumDisplayed;
            NumDisplayed = i;
            Sprites[loading].Draw(400, 300, NumDisplayed);
            SDL_RenderPresent(sdlRenderer);
            // TODO if(Old!=-1) Sprites[chargeur].Efface(400,300,Old,sdlVideo);
        }
    }
}

/*** Loads the sprite of a language ***/
/**************************************/
bool LoadLanguage()
{
    long L, P;
    int i;
    unsigned char *Buf;
    char PathFile[512];

    strcpy(PathFile, Languages[Pref.Language]);
    Utils::GetPath(PathFile);
    if (Utils::FileExists(PathFile) == false) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to find '%s'", Languages[Pref.Language]);
        return false;
    }
    L = Utils::LoadFile(PathFile, Buf);

    // Reads sprites
    P = 0;
    for (i = 0; i < NTexts; i++) {
        if (Sprites[T_level + i].N) {
            Sprites[T_level + i].Delete();
        }
        if (Sprites[T_level + i].Load(Buf, P) == false) {
            return false;
        }
    }
    delete[] Buf; // Frees memory from the sprite file

    return true;
}

/*** Loads the game's sprites ***/
/********************************/
bool LoadSprites()
{
    long L, P;
    int i;
    unsigned char *Buf;
    int NSp;

    char PathFile[512] = "language.dat";
    Utils::GetPath(PathFile);

    // Initialize characters table for texts
    for (i = 0; i < 256; i++) {
        TextTable[i] = -1;
    }
    i = 0;
    while (TextOrder[i] != 0) {
        TextTable[(int)(TextOrder[i])] = i;
        i++;
    }
    i = 0;
    while (TextOrder2[i] != 0) {
        TextTable[(int)(TextOrder2[i])] = i;
        i++;
    }

    // *** Loads the languages file ***
    // ********************************
    if (Utils::FileExists(PathFile) == false) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to find 'language.dat'");
        return false;
    }
    L = Utils::LoadFile(PathFile, Buf);

    // Read the sprites count
    NSp = (int)(Buf[0]) * 256 + (int)(Buf[1]);
    NSp += N_SPRITESFOND + 2;
    NTexts = (int)(Buf[2]) * 256 + (int)(Buf[3]);
    Pref.NLanguages = (int)(Buf[4]) * 256 + (int)(Buf[5]);

    NSprites = NSp + NTexts + Pref.NLanguages;
    Sprites = new Sprite[NSprites];

    // Fetch languages names
    P = 6;
    for (i = 0; i < Pref.NLanguages; i++) {
        strcpy(Languages[i], (char *)(Buf + P));
        P += strlen((char *)(Buf + P)) + 1;
    }

    // Load languages sprite
    for (i = 0; i < Pref.NLanguages; i++) {
        if (Sprites[T_Language + i].Load(Buf, P) == false) {
            return false;
        }
    }

    if (Sprites[loading].Load(Buf, P) == false) {
        return false; // Loading sprite
    }
    shouldDrawLoading = true; // can display loading sprite

    delete[] Buf;

    // *** loads sprites file ***
    // **************************
    strcpy(PathFile, "sprites.dat");
    Utils::GetPath(PathFile);
    if (Utils::FileExists(PathFile) == false) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to find 'sprites.dat'");
        return false;
    }
    L = Utils::LoadFile(PathFile, Buf);

    // Reads sprites
    P = 0;
    for (i = 0; i < NSp; i++) {
        DrawLoading();
        switch (i) {
        case fgame:
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

    delete[] Buf; // Frees memory from the sprites file

    // *** Loads language ***
    // **********************
    if (Pref.Language != -1) {
        LoadLanguage();
    }

    shouldDrawLoading = false; // Disable loading sprite
    return true;
}

/*** Returns the length of a number ***/
/**************************************/
int NumberLength(int C)
{
    int l = 0;

    do {
        l += Sprites[digits].Dim[(C % 10)].L;
        C /= 10;
        if (C) {
            l += GAP_BETWEEN_NUMBERS;
        }
    } while (C);

    return l;
}

/*** Returns the length of a string ***/
/**************************************/
int StringLength(char *Text)
{
    int i = 0;
    int l = 0;
    int Le;

    while (Text[i] != 0) {
        Le = (int)(Text[i]);
        if (TextTable[Le] != -1) {
            l += Sprites[letters].Dim[(TextTable[Le])].L;
            if (Text[i + 1] != 0) {
                l += GAP_BETWEEN_LETTERS;
            }
        }
        else {
            if (Le == (int)(' ')) {
                l += SPACE_LENGTH;
            }
        }

        i++;
    }

    return l;
}

/*** Checks if a char exists ***/
/*******************************/
bool CharExist(char C)
{
    if ((int)(C) < 0) {
        return false;
    }
    if (C == ' ') {
        return true;
    }
    if (TextTable[(int)(C)] != -1) {
        return true;
    }
    return false;
}
/*** Displays a number ***/
/*************************/
void DrawNumber(int x, int y, int Number, SDL_Texture *Background)
{
    int const l = NumberLength(Number);

    x += l / 2;
    do {
        Sprites[digits].Draw(x - (Sprites[digits].Dim[(Number % 10)].L) / 2, y, Number % 10, Background);
        x -= Sprites[digits].Dim[(Number % 10)].L + GAP_BETWEEN_NUMBERS;
        Number /= 10;
    } while (Number);
}

/*** Display a string ***/
/************************/
void DrawString(int x, int y, const char *Text, SDL_Texture *background)
{
    int i = 0;
    int Le;

    // TODO Handle here unicode
    while (Text[i] != 0) {
        Le = (int)(Text[i]);

        if (TextTable[Le] != -1) { // If known char
            Le = TextTable[Le];
            Sprites[letters].Draw(x + (Sprites[letters].Dim[Le].L / 2), y, Le, background);
            x += Sprites[letters].Dim[Le].L + GAP_BETWEEN_LETTERS;
        }
        else { // if there's a space
            if (Le == (int)(' ')) {
                x += SPACE_LENGTH - GAP_BETWEEN_LETTERS;
            }
        }

        i++;
    }
}

/*** Display text in a language ***/
/**********************************/
void DrawText(int x, int y, e_Sprite Text, SDL_Texture *Background)
{
    Sprites[Text].Draw(x, y, 0, Background);
}

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

/*** Load Sprites ***/
/********************/
bool Sprite::Load(unsigned char *Buf, long &P)
{
    int i, j;
    unsigned char *B;
    unsigned long ul = 1; // Checks processor type
    unsigned char *pul = (unsigned char *)(&ul);

    // Gets Number of sprites
    N = (int)(Buf[P]) * 256 + (int)(Buf[P + 1]);
    P += 2;
    Dim = new s_Dim[N];
    Image = new SDL_Texture *[N];

    // Read all sprites
    for (i = 0; i < N; i++) {
        // Displays loading
        DrawLoading();

        // Reads dimensions
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

        // Build surface
        SDL_Surface *surface = SDL_CreateRGBSurface(0, Dim[i].L, Dim[i].H, Dim[i].bpp * 8,
                                                    0xff, 0xff00, 0xff0000, 0xff000000 * (Dim[i].bpp - 3));
        if (surface == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create the SDL Surface");
            return false;
        }

        // Copy pixels
        SDL_LockSurface(surface);
        B = (unsigned char *)surface->pixels;

        if (pul[0] == 0) { // Processor type Power PC, 68000, ..
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

/*** Displaying sprite ***/
/*************************/
void Sprite::Draw(int X, int Y, int NumSpr, SDL_Texture *Background) const
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
/***********************************************/
void Sprite::PrintRope(int dx, int dy, int fx, int fy)
{
    SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 0);
    SDL_RenderDrawLine(sdlRenderer, dx, dy, fx, fy);
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 0);
}

/*** Allocates a new empty sprite ***/
/************************************/
bool Sprite::New(int Lx, int Ly)
{
    Delete(); // Delete "just in case"

    N = 1;
    Dim = new s_Dim[N];
    Image = new SDL_Texture *[N];

    Dim[0].L = Lx;
    Dim[0].H = Ly;
    Dim[0].cx = Lx / 2;
    Dim[0].cy = Ly / 2;
    Dim[0].bpp = 3; // No transparency

    // Builds surface
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

/*** Deletes a sprite ***/
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
