/*
 *  © 2020, Chris Harlow. All rights reserved.
 *  
 *  This file is part of Asbelos DCC API
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CommandStation.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef DIAG_h
#define DIAG_h

#include "StringFormatter.h"
#define DIAG  StringFormatter::diag
#define LCD   StringFormatter::lcd


// > grbba
enum Transport {
    _WIFI,
    _ETHERNET
};
// Current connection when recieveing commands over either transport
struct Connection {
    static Transport type;  // WIFI / ETHERNET
    static byte id;         // either the wifi id / socket no
};

// when a latch command has been recieved we copy here the current connection
// add functions for getting the raw socket and / wifistream to send the CIPSend command beofer calling the diag message

struct Latch {
    static Transport type;  // WIFI / ETHERNET
    static byte id;         // either the wifi id / socket no
};

// grbba >

#endif
