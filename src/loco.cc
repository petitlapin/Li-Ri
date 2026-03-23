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

extern sNewPreference Pref;
extern int currentTime;
extern int MaskK;

int AddDir[] = { -1, 1, -LT, LT };

Loco::Loco(Audio &audio) :
    m_audio(audio)
{
    Init(LT / 2 + HT / 2 * LT, D_Right);
}

/*** Initialize locomotive ***/
/*****************************/
void Loco::Init(int Pos, int Direction)
{
    int i;
    unsigned char Ar = 0;

    LocoPos = 0; // Points toward first tile
    IntersectPos = -1;
    Speed = Reduce = Extend = 0; // No extending
    Dead = -1;
    Win = false;
    Pref.WagonGap = WAGON_GAP_AVERAGE;

    // Initializing variables
    for (i = 0; i < 256; i++) {
        PosWagon[i].SprStart = 0;
    }

    // Look for the front tile
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

    // Initializing Level and locomotive
    D = D_Case * 1.99; // Position the head in front of the locomotive

    T[LocoPos].P = Pos;
    T[LocoPos].Arrive = Ar;
    T[LocoPos].Exit = Direction;
    T[LocoPos].D = D_Case;

    // Going forward one tile
    Go(Direction);
    Go(Direction);

    // Initializing the locomotive and its coal wagon
    NWagon = 2;
    Wagon[0] = locomotive;
    Wagon[1] = coal_wagon;
    PosWagon[0].SprStart = PosWagon[1].SprStart = N_SPR_START;
    PosWagon[0].dx = PosWagon[0].dy = -10;
    PosWagon[0].fx = PosWagon[0].fy = -10;
    PosWagon[1].dx = PosWagon[1].dy = -10;
    PosWagon[1].fx = PosWagon[1].fy = -10;

    MemoDuration = 0;
}

void Loco::Display(Screen &screen)
{
    float ltrain = 0;
    float p1, p2, a, ar, vx, vy;
    int x1, x2, y1, y2;
    int i, ns = 0;
    int cdx, cdy, cfx = 0, cfy = 0; // Attachment point for cables
    float lv;

    // Display all wagon/(cars)
    for (i = 0; i < NWagon; i++) {
        // Search wagon points
        switch (Wagon[i]) {
        case coal_wagon:
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

        // Calculate cars point's position
        FindPoint(D - p1, x1, y1);
        FindPoint(D - p2, x2, y2);

        PosWagon[i].dx = x1; // Save position points for future collision checks
        PosWagon[i].dy = y1;
        PosWagon[i].fx = x2;
        PosWagon[i].fy = y2;

        // Calculate the locomotive's angle of rotation and the number of the sprite
        if (x1 <= x2) { // Angle 0 through 180
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
        else { // Angle 180.001 through 359.999
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

        // store the center of the sprite
        x1 = (x1 + x2) / 2;
        y1 = (y1 + y2) / 2;

        // Ropes/Cables
        if (i > 0) { // if we need to display cable
            // Calculate the attachment point behind
            cdx = x1 - (int)(sin(ar) * lv);
            cdy = y1 - (int)(cos(ar) * lv);

            // Displays the cable
            screen.PrintCable(cdx, cdy, cfx, cfy);
        }
        // Calculate the attachment point for the next Wagon/car
        cfx = x1 - (int)(sin(ar + M_PI) * lv);
        cfy = y1 - (int)(cos(ar + M_PI) * lv);

        // Search the sprite's Number
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

        screen.PrintSprite(Wagon[i], ns, x1, y1);

        // If not done, display the start sequence of the wagon
        if (PosWagon[i].SprStart < N_SPR_START) {
            PosWagon[i].SprStart += MemoDuration * N_SPR_START / 750.0;
            if (PosWagon[i].SprStart < N_SPR_START) {
                screen.PrintSprite(new_wagon, (int)(PosWagon[i].SprStart), x1, y1);
            }
        }

        // Add gap between the wagons/cars
        ltrain += Pref.WagonGap;
    }
}

/*** Checking for items/options on a tile ***/
/*******************************************/
void Loco::TestTile(float Dist, long GameDuration, int *Level)
{
    int i;
    float DMoy;
    float Ec1, vx, vy, Ec2;

    // Check for collisions, half a tile in front of the locomotive
    DMoy = (T[LocoPos].D + T[LocoPos - 1].D) / 2.0;
    if (D <= DMoy && D + Dist >= DMoy) {
        // Check if on an item
        switch (Level[T[LocoPos].P]) {
        case C_Car: // New wagon/car
            m_audio.Play(sCar);
            Level[T[LocoPos].P] = 1; // Remove item from level
            Pref.Score += 5;
            AddLoco(); // Add a random wagon

            Win = true; // Check if it was the last wagon for win condition
            for (i = 0; i < LT * HT; i++) {
                if (Level[i] == C_Car) {
                    Win = false;
                }
            }
            if (Win) {
                Dead = currentTime + PAUSE_DURATION;
                m_audio.Play(sEnd);
            }
            break;
        case C_Expand: // Expand locomotive item
            m_audio.Play(sExpand);
            Level[T[LocoPos].P] = 1; // Remove item from map
            Pref.Score += 20;
            if (Reduce > GameDuration) {
                Reduce = GameDuration - 1;
            }
            else {
                Extend = GameDuration + EXTENSION_DURATION;
            }
            break;
        case C_Shrink: // Shrink locomotive item
            m_audio.Play(sShrink);
            Level[T[LocoPos].P] = 1; // remove item from map
            if (Extend > GameDuration) {
                Extend = GameDuration - 1;
            }
            else {
                Reduce = GameDuration + REDUCTION_DURATION;
            }
            break;
        case C_Speed: // Speed item
            m_audio.Play(sSpeed);
            Level[T[LocoPos].P] = 1; // remove item from map
            Pref.Score += 30;
            Speed = GameDuration + SPEED_DURATION;
            break;
        case C_Life: // Life item
            m_audio.Play(sLive);
            Level[T[LocoPos].P] = 1; // remove item from map
            Pref.Lives++;
            break;
        }

        // Collision check with another wagon/car
        for (i = 1; i < NWagon; i++) {
            vx = (float)(PosWagon[i].dx - PosWagon[0].dx);
            vy = (float)(PosWagon[i].dy - PosWagon[0].dy);
            Ec1 = vx * vx + vy * vy;
            vx = (float)(PosWagon[i].fx - PosWagon[0].dx);
            vy = (float)(PosWagon[i].fy - PosWagon[0].dy);
            Ec2 = vx * vx + vy * vy;

            // Death upon hitting wagon in range
            if (Dead < currentTime && (Ec1 < RAY_HIT || Ec2 <= RAY_HIT)) {
                m_audio.Play(sCrash);
                Pref.Lives--;
                Dead = currentTime + PAUSE_DURATION;
            }
        }
    }
}

/*** Makes locomotive move forward ***/
/*************************************/
void Loco::MoveForward(int Duration, long GameDuration, int *Key, int *Level)
{
    int i;
    float Dist = Pref.SpeedAverage * (float)(Duration) / 1000.0;

    MemoDuration = (float)(Duration);

    TestTile(Dist, GameDuration, Level);

    // Check if Locomotive should shrink
    if (Reduce > GameDuration) {
        if (Pref.WagonGap > WAGON_GAP_MIN) { // If locomotive must be shrinked
            Pref.WagonGap -= (float)(Duration) * (Pref.SpeedAverage * 0.8 / (float)(NWagon - 1)) / 1000.0;
            if (Pref.WagonGap < WAGON_GAP_MIN) {
                Pref.WagonGap = WAGON_GAP_MIN;
            }
        }
    }
    else { // If time passed
        if (Pref.WagonGap < WAGON_GAP_AVERAGE) { // If locomotive must be expanded
            Pref.WagonGap += (float)(Duration) * (Pref.SpeedAverage * 0.8 / (float)(NWagon)) / 1000.0;
            if (Pref.WagonGap > WAGON_GAP_AVERAGE) {
                Pref.WagonGap = WAGON_GAP_AVERAGE;
            }
        }
    }

    // Check if locomotive must be expanded
    if (Extend > GameDuration) {
        if (Pref.WagonGap < WAGON_GAP_MAX) { // If locomotive must be expanded
            Pref.WagonGap += (float)(Duration) * (Pref.SpeedAverage * 0.8 / (float)(NWagon)) / 1000.0;
            if (Pref.WagonGap > WAGON_GAP_MAX) {
                Pref.WagonGap = WAGON_GAP_MAX;
            }
        }
    }
    else { // If time passed
        if (Pref.WagonGap > WAGON_GAP_AVERAGE) { // If locomotive must be shrinked
            Pref.WagonGap -= (float)(Duration) * (Pref.SpeedAverage * 0.8 / (float)(NWagon - 1)) / 1000.0;
            if (Pref.WagonGap < WAGON_GAP_AVERAGE) {
                Pref.WagonGap = WAGON_GAP_AVERAGE;
            }
        }
    }

    // Check if locomotive speed must change
    if (Speed > GameDuration) {
        if (Pref.SpeedAverage < Pref.Speed * 2) { // if must accelerate
            Pref.SpeedAverage += (float)(Duration) / 40.0;
            if (Pref.SpeedAverage > Pref.Speed * 2) {
                Pref.SpeedAverage = Pref.Speed * 2;
            }
        }
    }
    else {
        if (Pref.SpeedAverage > Pref.Speed) { // if must slow down
            Pref.SpeedAverage -= (float)(Duration) / 40.0;
            if (Pref.SpeedAverage < Pref.Speed) {
                Pref.SpeedAverage = Pref.Speed;
            }
        }
    }

    // While distant tile isn't reached
    while (D + Dist > T[LocoPos].D) {
        Dist -= T[LocoPos].D - D; // Substract the remaining distance
        D = T[LocoPos].D;

        i = 0; // Search possible directions
        while (TestDir(Key[i], Level) == false) {
            i++;
        }

        Go(Key[i]); // Locomotive advances in the player's wanted direction

        FindArrow(Level, Key); // Search the position of the next intersection

        TestTile(Dist, GameDuration, Level); // Checks the tile for an item in case the game was really slow
    }

    D += Dist; // Add distance to total distance

    if (IntersectPos == -1) {
        FindArrow(Level, Key);
    }
}

/*** Search the position of the next intersection ***/
/****************************************************/
void Loco::FindArrow(int *Level, int *Key)
{
    int Exit = T[LocoPos].Exit;
    int MemoS;
    int NTracks;
    int x, y, Keypress;
    int i = 0;

    if (T[LocoPos].P != IntersectPos && IntersectPos != -1) {
        return; // If the crossing tile wasn't reached yet
    }

    IntersectPos = T[LocoPos].P; // Store the locomotive position

    do {
        // Goes to the next tile
        switch (Exit) {
        case D_Top:
            IntersectPos -= LT;
            EntryPos = D_Bottom;
            break;
        case D_Left:
            IntersectPos--;
            EntryPos = D_Right;
            break;
        case D_Bottom:
            IntersectPos += LT;
            EntryPos = D_Top;
            break;
        case D_Right:
            IntersectPos++;
            EntryPos = D_Left;
            break;
        }

        // Counts the different possible tracks
        MemoS = Exit;
        x = IntersectPos % LT;
        y = IntersectPos / LT;
        NTracks = 0;
        if (y > 0 && EntryPos != D_Top && Level[IntersectPos - LT] != 0) {
            Exit = D_Top;
            NTracks++;
        }
        if (y + 1 < HT && EntryPos != D_Bottom && Level[IntersectPos + LT] != 0) {
            Exit = D_Bottom;
            NTracks++;
        }
        if (x > 0 && EntryPos != D_Left && Level[IntersectPos - 1] != 0) {
            Exit = D_Left;
            NTracks++;
        }
        if (x + 1 < LT && EntryPos != D_Right && Level[IntersectPos + 1] != 0) {
            Exit = D_Right;
            NTracks++;
        }

    } while (NTracks == 1);

    // Set the exit direction as default direction
    while (Key[i] != MemoS) {
        i++;
    }
    if (i > 0 && !MaskK) { // Set the locomotive's direction as default direction
        Keypress = Key[i];
        while (i) {
            Key[i] = Key[i - 1];
            i--;
        }
        Key[0] = Keypress; // Store key/dir
    }
}

/*** Check if a direction/turn is possible ***/
/*********************************************/
bool Loco::TestDir(int FDir, int *Level)
{
    int PosBefore = T[LocoPos].P;
    int x, y;

    // Checks if directions aren't opposites
    if (T[LocoPos].Exit == D_Top && FDir == D_Bottom) {
        return false;
    }
    if (T[LocoPos].Exit == D_Bottom && FDir == D_Top) {
        return false;
    }
    if (T[LocoPos].Exit == D_Left && FDir == D_Right) {
        return false;
    }
    if (T[LocoPos].Exit == D_Right && FDir == D_Left) {
        return false;
    }

    PosBefore += AddDir[(T[LocoPos].Exit)]; // Position in turns

    // Check if there's a border
    x = PosBefore % LT;
    y = PosBefore / LT;
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

    PosBefore += AddDir[FDir]; // Set as future position

    if (Level[PosBefore] == 0) {
        return false; // Check if there's a rail
    }

    return true;
}

/*** Move the locomotive ***/
/***************************/
bool Loco::Go(int FutureDir)
{
    int PTab = T[LocoPos].P;
    int const Dir = T[LocoPos].Exit;
    int Mask;
    int i;

    // Test if there's a risk of exceeding track
    if (LocoPos == 255) {
        for (i = 0; i < 256 - 50; i++) {
            T[i].P = T[i + 50].P;
            T[i].D = T[i + 50].D;
            T[i].Arrive = T[i + 50].Arrive;
            T[i].Exit = T[i + 50].Exit;
        }
        LocoPos -= 50;
    }

    // Store next tile
    LocoPos++; // Move to next tile

    switch (Dir) {
    case D_Top:
        PTab -= LT;
        T[LocoPos].Arrive = D_Bottom;
        break;
    case D_Bottom:
        PTab += LT;
        T[LocoPos].Arrive = D_Top;
        break;
    case D_Left:
        PTab--;
        T[LocoPos].Arrive = D_Right;
        break;
    default:
        PTab++;
        T[LocoPos].Arrive = D_Left;
    }

    // Gives future direction and the number of the tile before
    T[LocoPos].P = PTab;
    T[LocoPos].Exit = FutureDir;

    // Calculates the distance
    Mask = T[LocoPos].Arrive * 4 + T[LocoPos].Exit;
    switch (Mask) {
    case (D_Left * 4 + D_Right): // if going straight forward
    case (D_Right * 4 + D_Left):
    case (D_Top * 4 + D_Bottom):
    case (D_Bottom * 4 + D_Top):
        T[LocoPos].D = T[LocoPos - 1].D + D_Case;
        break;
    default:
        T[LocoPos].D = T[LocoPos - 1].D + D_CaseR;
        return true;
    }

    return false;
}

/*** Searching a point on the map ***/
/************************************/
void Loco::FindPoint(float Dist, int &x, int &y)
{
    int NP = LocoPos;
    int P;
    float D_Rest;

    // Find the previous tile
    while (T[NP - 1].D > Dist) {
        NP--;
    }
    P = T[NP].P;

    // Calculate coordinates following the direction
    D_Rest = T[NP].D - Dist;
    if (D_Rest == 0) {
        D_Rest = D_Case / 1000.0; // Avoid problems with divisions
    }

    switch (T[NP].Arrive * 4 + T[NP].Exit) {
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

/*** Adds a random wagon to the locomotive ***/
/*********************************************/
void Loco::AddLoco()
{
    Wagon[NWagon] = (e_Sprite)(rand() % (car - logs_wagon) + logs_wagon);
    if (Wagon[NWagon] == Wagon[NWagon - 1]) { // Avoids adding the same sprite twice
        if (Wagon[NWagon] + 1 == car) {
            Wagon[NWagon] = logs_wagon;
        }
        else {
            Wagon[NWagon] = (e_Sprite)(Wagon[NWagon] + 1);
        }
    }
    PosWagon[NWagon].dx = PosWagon[NWagon].dy = -10;
    PosWagon[NWagon].fx = PosWagon[NWagon].fy = -10;
    NWagon++;
}
