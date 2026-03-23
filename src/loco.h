//      (_||_/
//      (    )       Loco class
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

#ifndef LOCO_DOM_
#define LOCO_DOM_

#include "sprite.h"

class Screen;
class Audio;

/*** Structs to store tile positions ***/
/***************************************/
struct s_TLoco
{
    int P; // Tile index
    float D; // Distance traveled
    unsigned char Arrive, Exit; // Arrival and Exit directions
};

struct s_PosWagon
{ // Stores positions in front and behind the locomotives for collision checks
    int dx, dy;
    int fx, fy;
    float SprStart; // if start display sequence is finished
};
#define N_SPR_START 50

class Loco
{
public:
    explicit Loco(Audio &audio);
    ~Loco() = default;

    void Init(int Pos, int Direction); // Initialize locomotive
    void Display(Screen &screen); // Display the locomotive
    void TestTile(float Dist, long GameDuration, int *Level); // Checking for items/options on a tile
    void MoveForward(int Duration, long GameDuration, int *Key, int *Level); // Makes locomotive move forward for Duration in ms
    void FindArrow(int *Level, int *Key); // Search the position of the next intersection
    bool TestDir(int FDir, int *Level); // Check if a direction/turn is possible
    void AddLoco(); // Adds a random car to the locomotive

    inline bool Go(int FutureDirection); // Move the locomotive
    inline void FindPoint(float Dist, int &x, int &y); // Searching a point on the map

    int Dead; // Stores current time + duration for a pause after losing game
    bool Win; // If we finished a level
    int EntryPos; // Entry position of the locomotive on a tile for arrows
    int IntersectPos; // Position of the incoming intersection to display the arrow at

private:
    long Reduce, Extend, Speed; // Stores the end clock to decide shrinking or expanding
    int LocoPos; // Locomotive's head position in the level
    float D; // Distance traveled by the locomotive
    struct s_TLoco T[256]; // Stores the route traveled by the locomotive, up to 256 tiles
    int NWagon; // Stores the number of wagons
    e_Sprite Wagon[256]; // Stores the wagons
    struct s_PosWagon PosWagon[256]; // Store wagon positions to check for collisions
    float MemoDuration; // Stores old duration to make start explosions move

    Audio &m_audio;
};

#endif
