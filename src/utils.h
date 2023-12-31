//      (_||_/
//      (    )       Utils functions
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

#ifndef _UTILS_DOM_
#define _UTILS_DOM_

class Utils
{
public:
    static bool FileExists(const char *Path); // Check if the file exists
    static long LoadFile(const char *Path, unsigned char *&Buf); // Load a file in memory
    static bool SaveFile(const char *Path, char *Buf, long L); // Save a file

    static void GetPath(char *Path); // Add the path to the filename depending on the OS (sprites, levels)

    static bool LoadPref(); // Load preferences
    static void SauvePref(); // Save preferences
};

#endif
