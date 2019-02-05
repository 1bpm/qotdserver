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
#include <time.h>
#include <libpq-fe.h>
#include <string.h>
#include "qotdserver.h"

static char *today_quote;
int day;
static PGconn *db;


static void clean_up() {
    PQfinish(db);
}

void clean_db() {
    clean_up();
}

int db_connect(Config *c) {    
    char *cstring_base = "host=%s dbname=%s user=%s password=%s";
    
    char *connection_string = malloc(
        strlen(c->db_host)
        + strlen(c->db_name)
        + strlen(c->db_user)
        + strlen(c->db_pass)
        + strlen(cstring_base)
        - 7
    );
    int cx;
    cx = sprintf(
        connection_string, 
        cstring_base, 
        c->db_host,
        c->db_name,
        c->db_user,
        c->db_pass
    );
    db = PQconnectdb(connection_string);
    
    if (PQstatus(db) != CONNECTION_OK) {
        char *template = "Connection to database failed: %s\n";
        char *message = PQerrorMessage(db);
        char *error = malloc(strlen(template) + strlen(message));
        sprintf(error, template, message);
        write_log(error);
        clean_db();
        return 0;
    }
    return 1;
}



char *get_quote_from_db() {
    
    PGresult *result;
    const char* query = "SELECT quote || CASE WHEN author IS NOT NULL "
        "THEN '\n\n  - ' || author ELSE '' END || "
        "CASE WHEN source IS NOT NULL THEN ', ' || source ELSE '' END || '\n\n' "
        "FROM quote ORDER BY RANDOM() LIMIT 1;";
    
    result = PQexec(db, query);
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        char *template = "Query error: %s\n";
        char *message = PQerrorMessage(db);
        char *error = malloc(strlen(template) + strlen(message));
        sprintf(error, template, PQerrorMessage(db));
        write_log(error);
    } else {
        char *quote = PQgetvalue(result, 0, 0);
        return quote;
    }
    
}


char *get_quote(Config *c) {
    if (c->daily == 0) {
        return get_quote_from_db();
    } else {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        int time = tm.tm_year + tm.tm_mon + tm.tm_mday;
        if (time != day) {
            today_quote = get_quote_from_db();
            day = time;
        }
        return today_quote;
    }
}