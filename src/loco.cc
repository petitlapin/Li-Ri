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

#include <cmath>
#include <cstdlib>
#include "preference.h"
#include "loco.h"
#include "audio.h"
#include "screen.h"

#define PIQ (M_PI / 2.0)
#ifndef M_PI
#define M_PI 3.141592654
#endif

/***   Global varibles  ***/
/**************************/
extern sNewPreference Pref;
extern int Horloge;
extern int MasqueK;

int AddDir[] = { -1, 1, -LT, LT };

/*** Constructor and destructor ***/
/**********************************/
Loco::Loco(Audio &audio) :
    m_audio(audio)
{
    Init(LT / 2 + HT / 2 * LT, D_Right);
}

/*** Locomotive initialisation ***/
/*********************************/
void Loco::Init(int Pos, int Direction)
{
    int i;
    unsigned char Ar = 0;

    PLoco = 0;
    PInter = -1;
    Vitesse = Reduit = Alonge = 0;
    Mort = -1;
    Gagne = false;
    Pref.EcartWagon = ECARTWAGON_MOY;

    // Varibles initilization
    for (i = 0; i < 256; i++) {
        PosWagon[i].SprStart = 0;
    }

    // Look at direction
    switch (Direction) {
    case D_Top:
        Pos += LT;
        Ar = D_Bottom;
        break;
    case D_Bottom:
        Pos -= LT;
        Ar = D_Top;
        break;
    case D_Left:
        Pos++;
        Ar = D_Right;
        break;
    case D_Right:
        Pos--;
        Ar = D_Left;
        break;
    }

    D = D_Case * 1.99;

    T[PLoco].P = Pos;
    T[PLoco].Arrive = Ar;
    T[PLoco].Sortie = Direction;
    T[PLoco].D = D_Case;

    // Moves forward one space
    Go(Direction);
    Go(Direction);

    // Initialize the locomotive and its coal wagon
    NWagon = 2;
    Wagon[0] = locomotive;
    Wagon[1] = charbon;
    PosWagon[0].SprStart = PosWagon[1].SprStart = N_SPR_START;
    PosWagon[0].dx = PosWagon[0].dy = -10;
    PosWagon[0].fx = PosWagon[0].fy = -10;
    PosWagon[1].dx = PosWagon[1].dy = -10;
    PosWagon[1].fx = PosWagon[1].fy = -10;

    MemoDuree = 0;
}

/*** Render locomotive ***/
/*************************/
void Loco::Display(Screen &Ec)
{
    float ltrain = 0;
    float p1, p2, a, ar, vx, vy;
    int x1, x2, y1, y2;
    int i, ns = 0;
    int cdx, cdy, cfx = 0, cfy = 0; // Cable attachment points
    float lv;

    // Render wagons
    for (i = 0; i < NWagon; i++) {
        // Find the points on the wagons
        switch (Wagon[i]) {
        case charbon:
            p1 = ltrain + 11;
            p2 = ltrain + 36 - 11;
            ltrain += 36.0;
            lv = 18;
            break;
        default:
            p1 = ltrain + 11;
            p2 = ltrain + 40 - 11;
            ltrain += 40.0;
            lv = 20;
        }

        // Calculate the position of the points
        FindPoint(D - p1, x1, y1);
        FindPoint(D - p2, x2, y2);

        PosWagon[i].dx = x1; // Saves the position of the points for future collision testing
        PosWagon[i].dy = y1;
        PosWagon[i].fx = x2;
        PosWagon[i].fy = y2;

        // Calculate locomotive rotation angle and sprite number
        if (x1 <= x2) { // Angle 0 - 180
            vy = (float)(x2 - x1);
            vx = (float)(y2 - y1);
            if (vx != 0) {
                a = atan(vy / vx) * 180.0 / M_PI;
            }
            else {
                a = 90.0;
            }
            if (vx < 0) {
                a = 180.0 + a;
            }
        }
        else { // Angle 180 - 359
            vy = (float)(x1 - x2);
            vx = (float)(y1 - y2);
            if (vx != 0) {
                a = atan(vy / vx) * 180.0 / M_PI + 180.0;
            }
            else {
                a = 270.0;
            }
            if (vx < 0) {
                a = 180.0 + a;
            }
        }
        ar = a * M_PI / 180.0;

        // Take the center of the sprite
        x1 = (x1 + x2) / 2;
        y1 = (y1 + y2) / 2;

        // Render cable
        if (i > 0) { // If it should render cable
            // Calculate the attachment point
            cdx = x1 - (int)(sin(ar) * lv);
            cdy = y1 - (int)(cos(ar) * lv);

            // Render cable
            Ec.PrintCable(cdx, cdy, cfx, cfy);
        }
        // Calcule le crocher de fin pour le prochaine wagon
        cfx = x1 - (int)(sin(ar + M_PI) * lv);
        cfy = y1 - (int)(cos(ar + M_PI) * lv);

        // Find sprite number
        switch ((int)a) {
        case 0: // Up
            ns = (int)(y1 + D_Case / 2) % (int)D_Case;
            break;
        case 180: // Down
            ns = (int)(y1 + D_Case / 2) % (int)D_Case + 40;
            break;
        case 90: // Left
            ns = (int)(x1 + D_Case / 2) % (int)D_Case + 80;
            break;
        case 270: // Right
            ns = (int)(x1 + D_Case / 2) % (int)D_Case + 120;
            break;
        default:
            ns = (int)(a * 160.0 / 360.0);
            if (ns >= 160) {
                ns = 0;
            }
            ns += 160;
        }

        Ec.PrintSprite(Wagon[i], ns, x1, y1);

        if (PosWagon[i].SprStart < N_SPR_START) {
            PosWagon[i].SprStart += MemoDuree * N_SPR_START / 750.0;
            if (PosWagon[i].SprStart < N_SPR_START) {
                Ec.PrintSprite(nouveau_wagon, (int)(PosWagon[i].SprStart), x1, y1);
            }
        }

        // Place the gap between the wagons
        ltrain += Pref.EcartWagon;
    }
}

/*** Colision test ***/
/*********************/
void Loco::TestCase(float Dist, long DureeJeu, int *Tableau)
{
    int i;
    float DMoy;
    float Ec1, vx, vy, Ec2;

    // Test if it crosses the middle of a cell for collision testing
    DMoy = (T[PLoco].D + T[PLoco - 1].D) / 2.0;
    if (D <= DMoy && D + Dist >= DMoy) {
        // Test if on an option
        switch (Tableau[T[PLoco].P]) {
        case C_Wagon: // A new wagon
            m_audio.Play(sWagon);
            Tableau[T[PLoco].P] = 1;
            Pref.Score += 5;
            AddLoco(); // Add random wagon

            Gagne = true; // Test if the last wagon
            for (i = 0; i < LT * HT; i++) {
                if (Tableau[i] == C_Wagon) {
                    Gagne = false;
                }
            }
            if (Gagne) {
                Mort = Horloge + DUREE_PAUSE;
                m_audio.Play(sEnd);
            }
            break;
        case C_Allonge: // Allonge locomotive
            m_audio.Play(sEtire);
            Tableau[T[PLoco].P] = 1;
            Pref.Score += 20;
            if (Reduit > DureeJeu) {
                Reduit = DureeJeu - 1;
            }
            else {
                Alonge = DureeJeu + DUREE_ALONGE;
            }
            break;
        case C_Reduit: // Reduit locomotive
            m_audio.Play(sReduit);
            Tableau[T[PLoco].P] = 1;
            if (Alonge > DureeJeu) {
                Alonge = DureeJeu - 1;
            }
            else {
                Reduit = DureeJeu + DUREE_REDUIT;
            }
            break;
        case C_Speed: // Speed up locomotive
            m_audio.Play(sSpeed);
            Tableau[T[PLoco].P] = 1;
            Pref.Score += 30;
            Vitesse = DureeJeu + DUREE_VITESSE;
            break;
        case C_Live: // Si Vie
            m_audio.Play(sLive);
            Tableau[T[PLoco].P] = 1; // efface l'option
            Pref.NVie++;
            break;
        }

        // Test colision with other wagon
        for (i = 1; i < NWagon; i++) {
            vx = (float)(PosWagon[i].dx - PosWagon[0].dx);
            vy = (float)(PosWagon[i].dy - PosWagon[0].dy);
            Ec1 = vx * vx + vy * vy;
            vx = (float)(PosWagon[i].fx - PosWagon[0].dx);
            vy = (float)(PosWagon[i].fy - PosWagon[0].dy);
            Ec2 = vx * vx + vy * vy;

            // If signal
            if (Mort < Horloge && (Ec1 < RAYON_TOUCHE || Ec2 <= RAYON_TOUCHE)) {
                m_audio.Play(sCrash);
                Pref.NVie--;
                Mort = Horloge + DUREE_PAUSE;
            }
        }
    }
}

/*** Move the locomotive forward ***/
/***********************************/
void Loco::Avance(int Duree, long DureeJeu, int *Touche, int *Tableau)
{
    int i;
    float Dist = Pref.VitesseMoy * (float)(Duree) / 1000.0;

    MemoDuree = (float)(Duree);

    TestCase(Dist, DureeJeu, Tableau);

    // Test if it needs to reduce the wagon
    if (Reduit > DureeJeu) {
        if (Pref.EcartWagon > ECARTWAGON_MIN) { // If must reduce
            Pref.EcartWagon -= (float)(Duree) * (Pref.VitesseMoy * 0.8 / (float)(NWagon - 1)) / 1000.0;
            if (Pref.EcartWagon < ECARTWAGON_MIN) {
                Pref.EcartWagon = ECARTWAGON_MIN;
            }
        }
    }
    else { // If time has passed
        if (Pref.EcartWagon < ECARTWAGON_MOY) { // If the wagon needs to be lengthened
            Pref.EcartWagon += (float)(Duree) * (Pref.VitesseMoy * 0.8 / (float)(NWagon)) / 1000.0;
            if (Pref.EcartWagon > ECARTWAGON_MOY) {
                Pref.EcartWagon = ECARTWAGON_MOY;
            }
        }
    }

    // Test if the wagon needs to be lengthened
    if (Alonge > DureeJeu) {
        if (Pref.EcartWagon < ECARTWAGON_MAX) { // If it needs to be extended
            Pref.EcartWagon += (float)(Duree) * (Pref.VitesseMoy * 0.8 / (float)(NWagon)) / 1000.0;
            if (Pref.EcartWagon > ECARTWAGON_MAX) {
                Pref.EcartWagon = ECARTWAGON_MAX;
            }
        }
    }
    else { // If time has passed
        if (Pref.EcartWagon > ECARTWAGON_MOY) { // If the wagon needs to be lengthened
            Pref.EcartWagon -= (float)(Duree) * (Pref.VitesseMoy * 0.8 / (float)(NWagon - 1)) / 1000.0;
            if (Pref.EcartWagon < ECARTWAGON_MOY) {
                Pref.EcartWagon = ECARTWAGON_MOY;
            }
        }
    }

    // Test si doit modifier la vitesse de la loco
    if (Vitesse > DureeJeu) {
        if (Pref.VitesseMoy < Pref.Vitesse * 2) { // Si doit accelerer
            Pref.VitesseMoy += (float)(Duree) / 40.0;
            if (Pref.VitesseMoy > Pref.Vitesse * 2) {
                Pref.VitesseMoy = Pref.Vitesse * 2;
            }
        }
    }
    else {
        if (Pref.VitesseMoy > Pref.Vitesse) { // Si doit ralentir
            Pref.VitesseMoy -= (float)(Duree) / 40.0;
            if (Pref.VitesseMoy < Pref.Vitesse) {
                Pref.VitesseMoy = Pref.Vitesse;
            }
        }
    }

    // Tand que dépasse la case en distante
    while (D + Dist > T[PLoco].D) {
        Dist -= T[PLoco].D - D; // Enleve la distance restant à parcourir
        D = T[PLoco].D;

        i = 0; // Cherche la direction possible
        while (TestDir(Touche[i], Tableau) == false) {
            i++;
        }

        Go(Touche[i]); // Fait avancer le loco suivant le désir du joueur

        DoFleche(Tableau, Touche); // Recherche la position de la futur intersection

        TestCase(Dist, DureeJeu, Tableau); // Test la case au cas ou le jeu est vraiment lent
    }

    D += Dist; // Met à la bonne position finale

    if (PInter == -1) {
        DoFleche(Tableau, Touche);
    }
}

/*** Recherche la position de la futur intersection ***/
/******************************************************/
void Loco::DoFleche(int *Tableau, int *Touche)
{
    int Sortie = T[PLoco].Sortie;
    int MemoS;
    int NVoie;
    int x, y, Tou;
    int i = 0;

    if (T[PLoco].P != PInter && PInter != -1) {
        return; // Si pas encore arrivé sur la case de croisement
    }

    PInter = T[PLoco].P; // Prend la position de la loco

    do {
        // Vas à la nouvelle case
        switch (Sortie) {
        case D_Top:
            PInter -= LT;
            PEntree = D_Bottom;
            break;
        case D_Left:
            PInter--;
            PEntree = D_Right;
            break;
        case D_Bottom:
            PInter += LT;
            PEntree = D_Top;
            break;
        case D_Right:
            PInter++;
            PEntree = D_Left;
            break;
        }

        // Compte les voies disponibles.
        MemoS = Sortie;
        x = PInter % LT;
        y = PInter / LT;
        NVoie = 0;
        if (y > 0 && PEntree != D_Top && Tableau[PInter - LT] != 0) {
            Sortie = D_Top;
            NVoie++;
        }
        if (y + 1 < HT && PEntree != D_Bottom && Tableau[PInter + LT] != 0) {
            Sortie = D_Bottom;
            NVoie++;
        }
        if (x > 0 && PEntree != D_Left && Tableau[PInter - 1] != 0) {
            Sortie = D_Left;
            NVoie++;
        }
        if (x + 1 < LT && PEntree != D_Right && Tableau[PInter + 1] != 0) {
            Sortie = D_Right;
            NVoie++;
        }

    } while (NVoie == 1);

    // Prend la direction de sortie comme direction par defaut
    while (Touche[i] != MemoS) {
        i++;
    }
    if (i > 0 && !MasqueK) { // Prend la direction de la loco comme direction par defaut
        Tou = Touche[i];
        while (i) {
            Touche[i] = Touche[i - 1];
            i--;
        }
        Touche[0] = Tou; // Mémorise la touche
    }
}

/*** Test si une direction est possible ***/
/******************************************/
bool Loco::TestDir(int FDir, int *Tableau)
{
    int PAvant = T[PLoco].P;
    int x, y;

    // Test si les directions ne sont pas opposées
    if (T[PLoco].Sortie == D_Top && FDir == D_Bottom) {
        return false;
    }
    if (T[PLoco].Sortie == D_Bottom && FDir == D_Top) {
        return false;
    }
    if (T[PLoco].Sortie == D_Left && FDir == D_Right) {
        return false;
    }
    if (T[PLoco].Sortie == D_Right && FDir == D_Left) {
        return false;
    }

    PAvant += AddDir[(T[PLoco].Sortie)]; // Position dans virage

    // Test si un bord
    x = PAvant % LT;
    y = PAvant / LT;
    if (FDir == D_Top && y == 0) {
        return false;
    }
    if (FDir == D_Bottom && y + 1 == HT) {
        return false;
    }
    if (FDir == D_Left && x == 0) {
        return false;
    }
    if (FDir == D_Right && x + 1 == LT) {
        return false;
    }

    PAvant += AddDir[FDir]; // Position futur

    if (Tableau[PAvant] == 0) {
        return false; // Test si il y a un rail
    }

    return true;
}

/*** Deplace la locomotive ***/
/*****************************/
bool Loco::Go(int FuturDir)
{
    int PTab = T[PLoco].P;
    int const Dir = T[PLoco].Sortie;
    int Mask;
    int i;

    // Test si risque de dépassement et enleve une bonne partie
    if (PLoco == 255) {
        for (i = 0; i < 256 - 50; i++) {
            T[i].P = T[i + 50].P;
            T[i].D = T[i + 50].D;
            T[i].Arrive = T[i + 50].Arrive;
            T[i].Sortie = T[i + 50].Sortie;
        }
        PLoco -= 50;
    }

    // Mémorise la prochaine case
    PLoco++; // Passe à la case suivante

    switch (Dir) {
    case D_Top:
        PTab -= LT;
        T[PLoco].Arrive = D_Bottom;
        break;
    case D_Bottom:
        PTab += LT;
        T[PLoco].Arrive = D_Top;
        break;
    case D_Left:
        PTab--;
        T[PLoco].Arrive = D_Right;
        break;
    default:
        PTab++;
        T[PLoco].Arrive = D_Left;
    }

    // Donne futur direction et N° case avant
    T[PLoco].P = PTab;
    T[PLoco].Sortie = FuturDir;

    // Calcule la distance
    Mask = T[PLoco].Arrive * 4 + T[PLoco].Sortie;
    switch (Mask) {
    case (D_Left * 4 + D_Right): // Si vas tous droit
    case (D_Right * 4 + D_Left):
    case (D_Top * 4 + D_Bottom):
    case (D_Bottom * 4 + D_Top):
        T[PLoco].D = T[PLoco - 1].D + D_Case;
        break;
    default:
        T[PLoco].D = T[PLoco - 1].D + D_CaseR;
        return true;
    }

    return false;
}

/*** Recherche un point sur le parcour ***/
/*****************************************/
void Loco::FindPoint(float Dist, int &x, int &y)
{
    int NP = PLoco;
    int P;
    float D_Rest;

    // Recherche la case d'avant
    while (T[NP - 1].D > Dist) {
        NP--;
    }
    P = T[NP].P;

    // Calcule les coordonnée suivant la direction
    D_Rest = T[NP].D - Dist;
    if (D_Rest == 0) {
        D_Rest = D_Case / 1000.0; // Evite les erreurs de division
    }

    switch (T[NP].Arrive * 4 + T[NP].Sortie) {
    case (D_Left * 4 + D_Right):
        y = int(P / LT * D_Case + D_Case / 2);
        x = int(P % LT * D_Case + D_Case - D_Rest);
        break;
    case (D_Left * 4 + D_Top):
        x = int(P % LT * D_Case + cos(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        y = int(P / LT * D_Case + sin(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        break;
    case (D_Left * 4 + D_Bottom):
        x = int(P % LT * D_Case + cos(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        y = int(P / LT * D_Case + D_Case - sin(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        break;

    case (D_Right * 4 + D_Left):
        y = int(P / LT * D_Case + D_Case / 2);
        x = int(P % LT * D_Case + D_Rest);
        break;
    case (D_Right * 4 + D_Top):
        x = int(P % LT * D_Case + D_Case - cos(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        y = int(P / LT * D_Case + sin(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        break;
    case (D_Right * 4 + D_Bottom):
        x = int(P % LT * D_Case + D_Case - cos(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        y = int(P / LT * D_Case + D_Case - sin(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        break;

    case (D_Bottom * 4 + D_Top):
        x = int(P % LT * D_Case + D_Case / 2);
        y = int(P / LT * D_Case + D_Rest);
        break;
    case (D_Bottom * 4 + D_Left):
        x = int(P % LT * D_Case + sin(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        y = int(P / LT * D_Case + D_Case - cos(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        break;
    case (D_Bottom * 4 + D_Right):
        x = int(P % LT * D_Case + D_Case - sin(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        y = int(P / LT * D_Case + D_Case - cos(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        break;

    case (D_Top * 4 + D_Bottom):
        x = int(P % LT * D_Case + D_Case / 2);
        y = int(P / LT * D_Case + D_Case - D_Rest);
        break;
    case (D_Top * 4 + D_Left):
        x = int(P % LT * D_Case + sin(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        y = int(P / LT * D_Case + cos(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        break;
    case (D_Top * 4 + D_Right):
        x = int(P % LT * D_Case + D_Case - sin(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        y = int(P / LT * D_Case + cos(D_Rest * PIQ / D_CaseR) * (D_Case / 2.0));
        break;
    }
}

/*** Ajoute une loco au azrard ***/
/*********************************/
void Loco::AddLoco()
{
    Wagon[NWagon] = (e_Sprite)(rand() % (wagon - buches) + buches);
    if (Wagon[NWagon] == Wagon[NWagon - 1]) { // Evite 2 fois le meme wagon
        if (Wagon[NWagon] + 1 == wagon) {
            Wagon[NWagon] = buches;
        }
        else {
            Wagon[NWagon] = (e_Sprite)(Wagon[NWagon] + 1);
        }
    }
    PosWagon[NWagon].dx = PosWagon[NWagon].dy = -10;
    PosWagon[NWagon].fx = PosWagon[NWagon].fy = -10;
    NWagon++;
}
