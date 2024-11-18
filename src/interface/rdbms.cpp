#include <stdlib.h>
#include <string.h>

#ifdef USE_POSTGRES
#include <libpq-fe.h>
#endif
#ifdef USE_ODBC
#include <sql.h>
#include <sqlext.h>
#endif

#include "rdbms.h"

using namespace std;

#ifdef USE_POSTGRES
/* From src/include/catalog/pg_type.h */
/*
#define TIMESTAMPOID 1114
#define BYTEAOID 17
#define CHAROID 18
#define BPCHAROID 1042
#define VARCHAROID 1043
*/
#endif

#ifdef USE_POSTGRES
int postgres_connectdb(const string &dbname, const string &user,
		       const string &password, Rdbms_conn *rdbms_conn)
{
	string conninfo;
	
	conninfo = "user='";
	conninfo += user;
	conninfo += "' password='";
	conninfo += password;
	conninfo += "' dbname='";
	conninfo += dbname;
	conninfo += "'";

	rdbms_conn->conn.postgres.conn = PQconnectdb(conninfo.c_str());

	if (PQstatus(rdbms_conn->conn.postgres.conn) != CONNECTION_OK)
		return -1;

	return 0;
}
#endif

#ifdef USE_ODBC
int odbc_connectdb(const string &dbname, const string &user,
		   const string &password, Rdbms_conn *rdbms_conn)
{
	int r;

	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE,
		       &(rdbms_conn->conn.odbc.env));
	SQLSetEnvAttr(rdbms_conn->conn.odbc.env, SQL_ATTR_ODBC_VERSION,
		      (void *) SQL_OV_ODBC3, 0);
	SQLAllocHandle(SQL_HANDLE_DBC, rdbms_conn->conn.odbc.env,
		       &(rdbms_conn->conn.odbc.dbc));

	r = SQLConnect(rdbms_conn->conn.odbc.dbc,
		       (SQLCHAR *) dbname.c_str(), dbname.length(),
		       (SQLCHAR *) user.c_str(), user.length(),
		       (SQLCHAR *) password.c_str(), password.length());

	if (!SQL_SUCCEEDED(r))
		return -1;

	SQLAllocHandle(SQL_HANDLE_STMT, rdbms_conn->conn.odbc.dbc,
		       &(rdbms_conn->conn.odbc.stmt));

	return 0;
}
#endif

int rdbms_connectdb(int rdbms_driver, const string &dbname,
		    const string &user, const string &password,
		    Rdbms_conn *rdbms_conn)
{
	int r;

	switch (rdbms_driver) {
#ifdef USE_POSTGRES
	case RDBMS_DRIVER_POSTGRES:
		r = postgres_connectdb(dbname, user, password, rdbms_conn);
		break;
#endif
#ifdef USE_ODBC
	case RDBMS_DRIVER_ODBC:
		r = odbc_connectdb(dbname, user, password, rdbms_conn);
		break;
#endif
	default:
		r = -1;
	}

	if (r == 0)
		rdbms_conn->rdbms_driver = rdbms_driver;

	return r;
}

#ifdef USE_POSTGRES
int postgres_closedb(Rdbms_conn *rdbms_conn)
{
/*        PQclear(rs); */
        PQfinish(rdbms_conn->conn.postgres.conn);

	return 0;
}
#endif

#ifdef USE_ODBC
int odbc_closedb(Rdbms_conn *rdbms_conn)
{
	SQLEndTran(SQL_HANDLE_ENV, rdbms_conn->conn.odbc.env, SQL_ROLLBACK);
	SQLFreeHandle(SQL_HANDLE_STMT, rdbms_conn->conn.odbc.stmt);
	SQLDisconnect(rdbms_conn->conn.odbc.dbc);
	SQLFreeHandle(SQL_HANDLE_DBC, rdbms_conn->conn.odbc.dbc);
	SQLFreeHandle(SQL_HANDLE_ENV, rdbms_conn->conn.odbc.env);

	return 0;
}
#endif

int rdbms_closedb(Rdbms_conn *rdbms_conn)
{
	switch (rdbms_conn->rdbms_driver) {
#ifdef USE_POSTGRES
	case RDBMS_DRIVER_POSTGRES:
		return postgres_closedb(rdbms_conn);
#endif
#ifdef USE_ODBC
	case RDBMS_DRIVER_ODBC:
		return odbc_closedb(rdbms_conn);
#endif
	default:
		return -1;
	}
}

#ifdef USE_POSTGRES
int postgres_exec(Rdbms_conn *rdbms_conn, const string &sql,
		  Rdbms_result *rdbms_result)
{
	rdbms_result->result.postgres.result =
		PQexec(rdbms_conn->conn.postgres.conn, sql.c_str());

	rdbms_result->result.postgres.nfields =
		PQnfields(rdbms_result->result.postgres.result);
	rdbms_result->result.postgres.ntuples =
		PQntuples(rdbms_result->result.postgres.result);
	rdbms_result->result.postgres.tuple_count = -1;

	return 0;
}
#endif

#ifdef USE_ODBC
int odbc_exec(Rdbms_conn *rdbms_conn, const string &sql,
	      Rdbms_result *rdbms_result)
{
	int x;

	if (SQLExecDirect(rdbms_conn->conn.odbc.stmt,
			  (SQLCHAR *) sql.c_str(),
			  SQL_NTS) != SQL_SUCCESS)
		return -1;
	rdbms_result->result.odbc.stmt = &(rdbms_conn->conn.odbc.stmt);

	if (!SQL_SUCCEEDED(SQLNumResultCols(
				   *rdbms_result->result.odbc.stmt,
				   &(rdbms_result->result.odbc.nfields))))
		return -1;
	rdbms_result->result.odbc.data = (char *)
		malloc(rdbms_result->result.odbc.nfields * MAXFIELD);
	rdbms_result->result.odbc.ind = (SQLINTEGER *)
		malloc(rdbms_result->result.odbc.nfields *
		       sizeof (SQLINTEGER) );

	for (x = 0; x < rdbms_result->result.odbc.nfields; x++)
		SQLBindCol(*rdbms_result->result.odbc.stmt, x + 1, SQL_C_CHAR,
			   rdbms_result->result.odbc.data + (x * MAXFIELD),
			   MAXFIELD,
			   (SQLLEN *) &(rdbms_result->result.odbc.ind[x]));

	return 0;
}
#endif

int rdbms_exec(Rdbms_conn *rdbms_conn, const string &sql,
	       Rdbms_result *rdbms_result)
{
	int r;

	switch (rdbms_conn->rdbms_driver) {
#ifdef USE_POSTGRES
	case RDBMS_DRIVER_POSTGRES:
		r = postgres_exec(rdbms_conn, sql, rdbms_result);
		break;
#endif
#ifdef USE_ODBC
	case RDBMS_DRIVER_ODBC:
		r = odbc_exec(rdbms_conn, sql, rdbms_result);
		break;
#endif
	default:
		r = -1;
	}

	if (r == 0)
		rdbms_result->rdbms_driver = rdbms_conn->rdbms_driver;

	return r;
}

#ifdef USE_POSTGRES
int postgres_nfields(Rdbms_result *rdbms_result, int *nfields)
{
	*nfields = rdbms_result->result.postgres.nfields;

	return 0;
}
#endif

#ifdef USE_ODBC
int odbc_nfields(Rdbms_result *rdbms_result, int *nfields)
{
	*nfields = rdbms_result->result.odbc.nfields;

	return 0;
}
#endif

int rdbms_nfields(Rdbms_result *rdbms_result, int *nfields)
{
	switch (rdbms_result->rdbms_driver) {
#ifdef USE_POSTGRES
	case RDBMS_DRIVER_POSTGRES:
		return postgres_nfields(rdbms_result, nfields);
#endif
#ifdef USE_ODBC
	case RDBMS_DRIVER_ODBC:
		return odbc_nfields(rdbms_result, nfields);
#endif
	default:
		return -1;
	}
}

#ifdef USE_POSTGRES
int postgres_fname(Rdbms_result *rdbms_result, int column, string* fname)
{
	*fname = PQfname(rdbms_result->result.postgres.result, column);
	return 0;
}
#endif

#ifdef USE_ODBC
int odbc_fname(Rdbms_result *rdbms_result, int column, string* fname)
{
	char s[1024];

	SQLColAttribute(*(rdbms_result->result.odbc.stmt), column + 1,
			SQL_DESC_LABEL, s, 1024, NULL, NULL);
	*fname = s;

	return 0;
}
#endif

int rdbms_fname(Rdbms_result *rdbms_result, int column, string* fname)
{
	switch (rdbms_result->rdbms_driver) {
#ifdef USE_POSTGRES
	case RDBMS_DRIVER_POSTGRES:
		return postgres_fname(rdbms_result, column, fname);
#endif
#ifdef USE_ODBC
	case RDBMS_DRIVER_ODBC:
		return odbc_fname(rdbms_result, column, fname);
#endif
	default:
		return -1;
	}
}

#ifdef USE_POSTGRES
int postgres_ftype(Rdbms_result *rdbms_result, int column, long *ftype)
{
	*ftype = PQftype(rdbms_result->result.postgres.result, column);

	return 0;
}
#endif

#ifdef USE_ODBC
int odbc_ftype(Rdbms_result *rdbms_result, int column, long *ftype)
{
	SQLColAttribute(*(rdbms_result->result.odbc.stmt), column + 1,
			SQL_DESC_CONCISE_TYPE, NULL, 0, NULL, ftype);

	return 0;
}
#endif

int rdbms_ftype(Rdbms_result *rdbms_result, int column, long *ftype)
{
	switch (rdbms_result->rdbms_driver) {
#ifdef USE_POSTGRES
	case RDBMS_DRIVER_POSTGRES:
		return postgres_ftype(rdbms_result, column, ftype);
#endif
#ifdef USE_ODBC
	case RDBMS_DRIVER_ODBC:
		return odbc_ftype(rdbms_result, column, ftype);
#endif
	default:
		return -1;
	}
}

#ifdef USE_POSTGRES
int postgres_fetch(Rdbms_result *rdbms_result)
{
	rdbms_result->result.postgres.tuple_count++;
	if (rdbms_result->result.postgres.tuple_count <
	    rdbms_result->result.postgres.ntuples) {
		return 0;
	} else {
		return -1;
	}
}
#endif

#ifdef USE_ODBC
int odbc_fetch(Rdbms_result *rdbms_result)
{
	if (!SQL_SUCCEEDED(SQLFetch(*(rdbms_result->result.odbc.stmt))))
		return -1;

	return 0;
}
#endif

int rdbms_fetch(Rdbms_result *rdbms_result)
{
	switch (rdbms_result->rdbms_driver) {
#ifdef USE_POSTGRES
	case RDBMS_DRIVER_POSTGRES:
		return postgres_fetch(rdbms_result);
#endif
#ifdef USE_ODBC
	case RDBMS_DRIVER_ODBC:
		return odbc_fetch(rdbms_result);
#endif
	default:
		return -1;
	}
}

#ifdef USE_POSTGRES
int postgres_getvalue(Rdbms_result *rdbms_result, int column, string* value)
{
	*value = PQgetvalue(rdbms_result->result.postgres.result,
			    rdbms_result->result.postgres.tuple_count, column);
	return 0;
}
#endif

#ifdef USE_ODBC
int odbc_getvalue(Rdbms_result *rdbms_result, int column, string* value)
{
	if (rdbms_result->result.odbc.ind[column] > 0)
		*value = rdbms_result->result.odbc.data + (column * MAXFIELD);
	else
		*value = "";
	return 0;
}
#endif

int rdbms_getvalue(Rdbms_result *rdbms_result, int column, string* value)
{
	switch (rdbms_result->rdbms_driver) {
#ifdef USE_POSTGRES
	case RDBMS_DRIVER_POSTGRES:
		return postgres_getvalue(rdbms_result, column, value);
#endif
#ifdef USE_ODBC
	case RDBMS_DRIVER_ODBC:
		return odbc_getvalue(rdbms_result, column, value);
#endif
	default:
		return -1;
	}
}

#ifdef USE_POSTGRES
int postgres_close_result(Rdbms_result *rdbms_result)
{
	PQclear(rdbms_result->result.postgres.result);
	return 0;
}
#endif

#ifdef USE_ODBC
int odbc_close_result(Rdbms_result *rdbms_result)
{
	free(rdbms_result->result.odbc.ind);
	free(rdbms_result->result.odbc.data);
	return 0;
}
#endif

int rdbms_close_result(Rdbms_result *rdbms_result)
{
	switch (rdbms_result->rdbms_driver) {
#ifdef USE_POSTGRES
	case RDBMS_DRIVER_POSTGRES:
		return postgres_close_result(rdbms_result);
#endif
#ifdef USE_ODBC
	case RDBMS_DRIVER_ODBC:
		return odbc_close_result(rdbms_result);
#endif
	default:
		return -1;
	}
}
