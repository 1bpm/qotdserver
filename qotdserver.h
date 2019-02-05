/*
 qotdserver
Copyright (C) 2019 Richard Knight

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

#ifndef QOTDSERVER_H
#define QOTDSERVER_H

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

typedef struct {
    const char* db_host;
    const char* db_name;
    const char* db_user;
    const char* db_pass;
    int daily;
} Config;

Config config;


#endif /* QOTDSERVER_H */


