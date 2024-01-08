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

#include <cstdio>
#include <cstring>

#include <SDL2/SDL_filesystem.h> // for SDL_GetPrefPath
#include <SDL2/SDL_log.h> // for SDL_LogError, SDL_LOG_CATEGORY_APPL...
#include <SDL2/SDL_rwops.h> // for SDL_RWclose, SDL_RWFromFile, SDL_RW...
#include <SDL2/SDL_stdinc.h> // for SDL_free
#include <SDL2/SDL_pixels.h> // for SDL_PIXELFORMAT_ARGB8888
#include <SDL2/SDL_surface.h> // for SDL_CreateRGBSurface, SDL_FreeSurface

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
bool Utils::FileExists(const char *Path)
{
    SDL_RWops *file = SDL_RWFromFile(Path, "rb");

    if (file == nullptr) {
        return false;
    }

    SDL_RWclose(file);
    return true;
}

/*** Charge un fichier en Mémoire ***/
/************************************/
long Utils::LoadFile(const char *Path, unsigned char *&Buf)
{
    SDL_RWops *file = SDL_RWFromFile(Path, "rb");
    if (!file) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to open '%s'", Path);
        return -1;
    }

    long const L = SDL_RWsize(file);

    Buf = new unsigned char[L + 1];
    if (Buf == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Not enough memory");
        SDL_RWclose(file);
        return -1;
    }

    long Compt = L;
    unsigned char *Po = Buf;

    while (Compt > 1024) {
        AfficheChargeur();
        if (SDL_RWread(file, Po, 1, 1024) != 1024) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error while reading '%s'", Path);
            SDL_RWclose(file);
            delete[] Buf;
            return -1;
        }
        Compt -= 1024;
        Po += 1024;
    }

    if (Compt) { // Ne fait pas le test à cause d'un bug dans windows
        SDL_RWread(file, Po, 1, (unsigned int)Compt);
    }

    SDL_RWclose(file);
    return L;
}

/*** Sauve un Fichier ***/
/************************/
bool Utils::SaveFile(const char *Path, char *Buf, long L)
{
    FILE *file;

    file = fopen(Path, "w");
    if (!file) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to open '%s'", Path);
        return false;
    }

    while (L > 512) {
        if (fwrite(Buf, 1, 512, file) != 512) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to write in '%s'", Path);
            fclose(file);
            return false;
        }
        L -= 512;
        Buf += 512;
    }

    if (L > 0) {
        if (fwrite(Buf, 1, (size_t)L, file) != (size_t)L) {
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

    strcpy(Provi, Path);

#ifndef ANDROID
    if (DefPath[0]) {
        sprintf(Path, "%s%s", DefPath, Provi);
        if (Utils::FileExists(Path)) {
            return;
        }
    }
    sprintf(Path, "%s/%s", DATA_DIR, Provi);
    if (Utils::FileExists(Path)) {
        return;
    }
#endif

    // Android is directly the filename
    sprintf(Path, "%s", Provi);
    if (Utils::FileExists(Path)) {
        return;
    }

    sprintf(Path, "./%s", Provi);
    if (Utils::FileExists(Path)) {
        return;
    }
    sprintf(Path, "/usr/local/share/Li-ri/%s", Provi);
    if (Utils::FileExists(Path)) {
        return;
    }
    sprintf(Path, "/usr/share/Li-ri/%s", Provi);
    if (Utils::FileExists(Path)) {
        return;
    }
    sprintf(Path, "/usr/share/games/Li-ri/%s", Provi);
    if (Utils::FileExists(Path)) {
        return;
    }

    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to find '%s'", Provi);
}
#endif

#ifdef __AMIGAOS4__
// Version AmigaOS4
void Utils::GetPath(char *Path)
{
    char Provi[512];

    strcpy(Provi, Path);

    sprintf(Path, "PROGDIR:%s", Provi);
    if (Utils::FileExists(Path))
        return;

    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to find '%s'", Path);
    exit(-1);
}
#endif

#ifdef __APPLE__
// Version Mac OSX
void Utils::GetPath(char *Path)
{
    char Provi[512];

    strcpy(Provi, Path);

    sprintf(Path, "Li-ri.app/Contents/Resources/%s", Provi);
    if (Utils::FileExists(Path))
        return;

    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to find '%s'", Path);
    exit(-1);
}
#endif

#ifdef _WIN32
//  Version Windows , chemin directe
void Utils::GetPath(char *Path)
{
    char Provi[512];

    strcpy(Provi, Path);
    char *basePath = SDL_GetBasePath();
    sprintf(Path, "%s/%s", basePath, Provi);
    if (Utils::FileExists(Path)) {
        SDL_free(basePath);
        return;
    }
    SDL_free(basePath);
}
#endif

/*** Charge les préferences ***/
/******************************/
bool Utils::LoadPref()
{
    int L;
    unsigned char *Provi;

    char PathPref[512];
    char *PrefFolder = SDL_GetPrefPath("Li-Ri", "Li-Ri");
    sprintf(PathPref, "%sli-ri.pref", PrefFolder);

    SDL_free(PrefFolder);

    if (Utils::FileExists(PathPref)) {
        L = Utils::LoadFile(PathPref, Provi);
        if (L > 0) {
            memcpy((char *)&Pref, Provi, L);
            delete[] Provi;
            return true;
        }
    }

    return false;
}

/*** Sauve les preferences ***/
/*****************************/
void Utils::SauvePref()
{
    char PathPref[512];
    char *PrefFolder = SDL_GetPrefPath("Li-Ri", "Li-Ri");
    sprintf(PathPref, "%sli-ri.pref", PrefFolder);

    SDL_free(PrefFolder);
    Utils::SaveFile(PathPref, (char *)&Pref, sizeof(sPreference));
}

void Utils::doScreenshot(SDL_Renderer *renderer)
{
    static int screenshotNumber = 0;
    char screenshotName[80];
    int w, h;
    SDL_GetRendererOutputSize(renderer, &w, &h);
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 24,
                                                          SDL_PIXELFORMAT_RGB24);
    SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGB24, surface->pixels, surface->pitch);
    sprintf(screenshotName, "screenshot%i.bmp", screenshotNumber++);
    SDL_SaveBMP(surface, screenshotName);
    SDL_FreeSurface(surface);
}
