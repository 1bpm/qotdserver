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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "ini.h"
#include "qotdserver.h"
#include "logger.h"


void signal_handler(int sig){
    switch(sig){
        case SIGTERM:
            clean_db();
            write_log("SIGTERM received");
            close_logger();
            exit(0);
            break;
        case SIGKILL:
            clean_db();
            write_log("SIGKILL received");
            close_logger();
            exit(0);
            break;
    }
}

static int config_handler(void *user, const char *section, const char *name, 
                            const char *value) 
{
    Config *cnf = (Config*)user;
    
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("database", "user")) {
        cnf->db_user = strdup(value);
    } else if (MATCH("database", "password")) {
        cnf->db_pass = strdup(value);
    } else if (MATCH("database", "host")) {
        cnf->db_host = strdup(value);
    } else if (MATCH("database", "name")) {
        cnf->db_name = strdup(value);
    } else if (MATCH("general", "daily")) {
        cnf->daily = atoi(value);
    } else {
        return 0;
    }
    return 1;
}


int main(int argc, char** argv) 
{
    signal(SIGTERM, signal_handler);
    signal(SIGKILL, signal_handler);
    
    char *conf_path = "/etc/qotdserver.conf";
    
    
    if (ini_parse(conf_path, config_handler, &config) < 0) {
        printf("Can't load configuration file %s\n", conf_path);
        return (EXIT_FAILURE);
    }
    
    if (!init_logger(&config)) {
        printf("Can't open log file for writing\n");
        return (EXIT_FAILURE);
    }
    
    if (!db_connect(&config)) {
        return (EXIT_FAILURE);
    }
   
    write_log("Starting server");
    
    daemon(0, 0);
    serve();
        
}

