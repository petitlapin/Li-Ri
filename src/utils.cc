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
#include <string>

#include <SDL2/SDL_filesystem.h> // for SDL_GetPrefPath
#include <SDL2/SDL_log.h> // for SDL_LogError, SDL_LOG_CATEGORY_APPL...
#include <SDL2/SDL_rwops.h> // for SDL_RWclose, SDL_RWFromFile, SDL_RW...
#include <SDL2/SDL_stdinc.h> // for SDL_free
#include <SDL2/SDL_pixels.h> // for SDL_PIXELFORMAT_ARGB8888
#include <SDL2/SDL_surface.h> // for SDL_CreateRGBSurface, SDL_FreeSurface

#include "config.h"
#include "utils.h"
#include "preference.h"
#include "sprite.h"

#include "SimpleIni.h"

/*** Variables globales ***/
/**************************/
extern sNewPreference Pref;
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
#if (defined(__HAIKU__) || defined(__unix__) || defined(ANDROID)) && !defined(__AMIGAOS4__)
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
#endif

    // Android is directly the filename
    sprintf(Path, "%s", Provi);
    if (Utils::FileExists(Path)) {
        return;
    }

    sprintf(Path, "%s/%s", LIRI_DATA_DIR, Provi);
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
    if (Utils::FileExists(Path)) {
        return;
    }

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
    if (Utils::FileExists(Path)) {
        return;
    }

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

    char oldPathPref[512];
    char *PrefFolder = SDL_GetPrefPath("Li-Ri", "Li-Ri");
    sprintf(oldPathPref, "%sli-ri.pref", PrefFolder);

    char newPathPref[512];
    sprintf(newPathPref, "%sli-ri.ini", PrefFolder);

    SDL_free(PrefFolder);

    // Load new config file if it exists
    if (Utils::FileExists(newPathPref)) {
        CSimpleIniA ini(true); // true for unicode
        SI_Error rc = ini.LoadFile(newPathPref);
        const char *pv;
        pv = ini.GetValue("main", "fullscreen");
        if (pv) {
            Pref.FullScreen = std::stoi(pv);
        }
        pv = ini.GetValue("main", "locale");
        if (pv) {
            Pref.Langue = std::stoi(pv);
        }
        pv = ini.GetValue("main", "audioVolume");
        if (pv) {
            Pref.Volume = std::stof(pv);
        }
        pv = ini.GetValue("main", "musicVolume");
        if (pv) {
            Pref.VolumeM = std::stof(pv);
        }
        pv = ini.GetValue("main", "humanRightsQuiz");
        if (pv) {
            Pref.HumanRightsQuiz = std::stoi(pv);
        }
        pv = ini.GetValue("easy", "maxLevel");
        if (pv) {
            Pref.NiveauMax[0] = std::stof(pv);
        }
        pv = ini.GetValue("normal", "maxLevel");
        if (pv) {
            Pref.NiveauMax[1] = std::stof(pv);
        }
        pv = ini.GetValue("difficult", "maxLevel");
        if (pv) {
            Pref.NiveauMax[2] = std::stof(pv);
        }
        for (int i = 0; i < 8; ++i) {
            std::string scoreKey = "score_" + std::to_string(i);
            std::string nameKey = "name_" + std::to_string(i);
            if (pv) {
                pv = ini.GetValue("highscore", scoreKey.c_str());
            }
            Pref.Sco[i].Score = std::stoi(pv);
            if (pv) {
                pv = ini.GetValue("highscore", nameKey.c_str(), Pref.Sco[i].Name);
            }
            strncpy(Pref.Sco[i].Name, pv, 80);
        }
        return true;
    }

    // In case we don't have yet a file in the new format, we check if we don't have a file in the previous format to restore it.
    if (Utils::FileExists(oldPathPref)) {
        L = Utils::LoadFile(oldPathPref, Provi);
        if (L > 0) {
            sOldPreference oldPref;
            memcpy((char *)&oldPref, Provi, L);
            delete[] Provi;
            Pref.FullScreen = oldPref.FullScreen;
            Pref.Langue = oldPref.Langue;
            Pref.Volume = oldPref.Volume;
            Pref.VolumeM = oldPref.VolumeM;
            // Try to restore max difficulty from the level stored in conf
            Pref.NiveauMax[0] = oldPref.Difficulte == Easy ? oldPref.NiveauMax : 0;
            Pref.NiveauMax[1] = oldPref.Difficulte == Normal ? oldPref.NiveauMax : 0;
            Pref.NiveauMax[2] = oldPref.Difficulte == Hard ? oldPref.NiveauMax : 0;
            for (int i = 0; i < 8; ++i) {
                Pref.Sco[i].Score = oldPref.Sco[i].Score;
                strncpy(Pref.Sco[i].Name, oldPref.Sco[i].Name, 80);
            }
            // TODO Delete old file at some file
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
    sprintf(PathPref, "%sli-ri.ini", PrefFolder);

    SDL_free(PrefFolder);
    CSimpleIniA ini(true); // true for unicode
    ini.SetValue("main", "fullscreen", std::to_string(Pref.FullScreen).c_str());
    ini.SetValue("main", "locale", std::to_string(Pref.Langue).c_str());
    ini.SetValue("main", "audioVolume", std::to_string(Pref.Volume).c_str());
    ini.SetValue("main", "musicVolume", std::to_string(Pref.VolumeM).c_str());
    ini.SetValue("main", "humanRightsQuiz", std::to_string(Pref.HumanRightsQuiz).c_str());
    ini.SetValue("easy", "maxLevel", std::to_string(Pref.NiveauMax[0]).c_str());
    ini.SetValue("normal", "maxLevel", std::to_string(Pref.NiveauMax[1]).c_str());
    ini.SetValue("difficult", "maxLevel", std::to_string(Pref.NiveauMax[2]).c_str());
    for (int i = 0; i < 8; ++i) {
        std::string scoreKey = "score_" + std::to_string(i);
        std::string nameKey = "name_" + std::to_string(i);
        ini.SetValue("highscore", scoreKey.c_str(), std::to_string(Pref.Sco[i].Score).c_str());
        ini.SetValue("highscore", nameKey.c_str(), Pref.Sco[i].Name);
    }
    ini.SaveFile(PathPref);
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
