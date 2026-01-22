//      (_||_/
//      (    )       Sprite class
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

#ifndef SPRITE_DOM_
#define SPRITE_DOM_

#include <SDL3/SDL_render.h>

#define FCOLOR 128
#define ALPHA_OMBRE 125
#define ECART_ENTRE_CHIFFRE 1
#define ECART_ENTRE_LETTRE 2
#define LONGUEUR_ESPACE 30

/*** Definition de la structure Dim ***/
/**************************************/
struct s_Dim
{
    int L, H;
    int cx, cy; // Centre de l'image après recadrage
    int bpp; // Nombre de bytes par pixels 3 ou 4 avec l'alpha
};

/*** Nom des sprites ***/
/***********************/
enum e_Sprite {
    locomotive = 0,
    coal_wagon,
    logs_wagon,
    cargo_wagon,
    engine_wagon,
    cistern_wagon,
    wagon,
    pluslong,
    pluscourt,
    speed,
    life,
    new_wagon,
    logo_vie,
    rail,
    dir,
    lettres,
    digits,
    title,
    copyright,
    deco,
    cursor,
    arrow_left,
    arrow_right,
    arrows,
    earth,
    music,
    sound,
    fscreen,
    window,
    keys,
    gmenu,
    menu,
    fond_hr,
    fond_hrr,
    fond_menu,
    fond,

    fjeu,
    fmenu,
    rope,
    loading,

    T_level,
    T_lives,
    T_options,
    T_score,
    T_press_any_key,
    T_continue,
    T_moptions,
    T_exit_game,
    T_play,
    T_scores,
    T_better_scores,
    T_quit,
    T_new_game,
    T_old_level,
    T_menu,
    T_easy,
    T_normal,
    T_hard,
#ifndef DCHILDREN
    T_question,
    T_tart1,
    T_tart2,
    T_tart3,
    T_tart4,
    T_tart5,
    T_tart6,
    T_tart7,
    T_tart8,
    T_tart9,
    T_tart10,
    T_tart11,
    T_tart12,
    T_tart13,
    T_tart14,
    T_tart15,
    T_tart16,
    T_tart17,
    T_tart18,
    T_tart19,
    T_tart20,
    T_tart21,
    T_tart22,
    T_tart23,
    T_tart24,
    T_tart25,
    T_tart26,
    T_tart27,
    T_tart28,
    T_tart29,
    T_tart30,
    T_art1,
    T_art2,
    T_art3,
    T_art4,
    T_art5,
    T_art6,
    T_art7,
    T_art8,
    T_art9,
    T_art10,
    T_art11,
    T_art12,
    T_art13,
    T_art14,
    T_art15,
    T_art16,
    T_art17,
    T_art18,
    T_art19,
    T_art20,
    T_art21,
    T_art22,
    T_art23,
    T_art24,
    T_art25,
    T_art26,
    T_art27,
    T_art28,
    T_art29,
    T_art30,
#endif
    T_Langue,

    T_ENDTEXT
};

/*** Fonctions ***/
/*****************/
void DrawLoading(); // Affiche le chargeur sur la page de départ
bool LoadLanguage(); // Charge les sprites d'une langue
bool LoadSprites(); // Charge tous les sprites

int NumberLength(int C); // Retourne la longueur en pixels d'un nombre
int StringLength(char *Texte); // Retourne la longueur en pixels d'un texte
bool CharExist(char C); // Si un caracataire existe

void DrawNumber(int x, int y, int Nombre, SDL_Texture *Fond = nullptr); // Affiche un chiffre
void DrawString(int x, int y, char *Texte, SDL_Texture *Fond = nullptr); // Affiche une chaine de caractaire

void DrawText(int x, int y, e_Sprite Text, SDL_Texture *Fond = nullptr); // Affiche un text dans la langue

/*** Definition de la classe Sprite ***/
/**************************************/
class Sprite
{
public:
    Sprite() = default;
    ~Sprite();

    /*** Fonctions ***/
    bool Load(unsigned char *Buf, long &P); // Charge les images
    void Draw(int X, int Y, int NumSpr, SDL_Texture *Fond = nullptr) const; // Affiche un sprite
    void PrintRope(int dx, int dy, int fx, int fy);
    bool New(int Lx, int Ly); // Alloue un nouveau sprite vide sans transparence
    void Delete(); // Efface la mémoire du sprite

    /*** Variables ***/
    int N { 0 }; // Nombre de sprite
    SDL_Texture **Image { nullptr }; // Pointe sur les sprites
    s_Dim *Dim { nullptr }; // Dimensions des sprites
};

#endif
