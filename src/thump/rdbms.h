#ifndef _RDBMS_H
#define _RDBMS_H

#include <string>

using namespace std;

#define RDBMS_DRIVER_POSTGRES 1
#define RDBMS_DRIVER_ODBC 2

#define MAXFIELD 1048576

#ifdef USE_POSTGRES
typedef struct {
	PGconn *conn;
} Postgres_conn;
#endif

#ifdef USE_ODBC
typedef struct {
	SQLHENV env;
	SQLHDBC dbc;
	SQLHSTMT stmt;
} Odbc_conn;
#endif

typedef struct {
	int rdbms_driver;
	union {
#ifdef USE_POSTGRES
		Postgres_conn postgres;
#endif
#ifdef USE_ODBC
		Odbc_conn odbc;
#endif
	} conn;
} Rdbms_conn;

#ifdef USE_POSTGRES
typedef struct {
	PGresult *result;
	int nfields;
	int ntuples;
	int tuple_count;
} Postgres_result;
#endif

#ifdef USE_ODBC
typedef struct {
	SQLHSTMT *stmt;
	SQLSMALLINT nfields;
	char *data;
	SQLINTEGER *ind;
} Odbc_result;
#endif

typedef struct {
	int rdbms_driver;
	union {
#ifdef USE_POSTGRES
		Postgres_result postgres;
#endif
#ifdef USE_ODBC
		Odbc_result odbc;
#endif
	} result;
} Rdbms_result;

int rdbms_connectdb(int rdbms_driver, const string &dbname,
		    const string &user, const string &password,
		    Rdbms_conn *rdbms_conn);

int rdbms_closedb(Rdbms_conn *rdbms_conn);

int rdbms_exec(Rdbms_conn *rdbms_conn, const string &sql,
	       Rdbms_result *rdbms_result);

int rdbms_nfields(Rdbms_result *rdbms_result, int *nfields);

int rdbms_fname(Rdbms_result *rdbms_result, int column, string* fname);

int rdbms_ftype(Rdbms_result *rdbms_result, int column, long *ftype);

int rdbms_fetch(Rdbms_result *rdbms_result);

int rdbms_getvalue(Rdbms_result *rdbms_result, int column, string* value);

int rdbms_close_result(Rdbms_result *rdbms_result);

#endif
