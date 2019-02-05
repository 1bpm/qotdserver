/* inih -- simple .INI file parser

The "inih" library is distributed under the New BSD license:

Copyright (c) 2009, Ben Hoyt
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Ben Hoyt nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY BEN HOYT ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL BEN HOYT BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

https://github.com/benhoyt/inih

*/

#include "qotdserver.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE *logfile = NULL;


int init_logger(Config *cnf) {
    logfile = fopen("/var/log/qotdserver.log", "a+"); 
    if (logfile == NULL) { 
        return 0;
    }
    return 1;
}

void close_logger() {
    if (logfile != NULL) {
        fclose(logfile);
    }
}

void write_log(char* to_write) 
{
    if (logfile != NULL) {
        
        time_t rawtime;
        struct tm *timeinfo;
        int len = 27 + strlen(to_write);
        char buffer[len];

        time(&rawtime);
        timeinfo = localtime (&rawtime);

        strftime(buffer, 25, "%F %X: ", timeinfo);
        strcat(buffer, to_write);
        strcat(buffer, "\n");
        
        fprintf(logfile, buffer);
        fflush(logfile);
    }
        
}