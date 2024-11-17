#include <string>
#include <iostream>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <ctype.h>
#ifdef USE_POSTGRES
#include <libpq-fe.h>
#endif
#ifdef USE_ODBC
#include <sql.h>
#include <sqlext.h>
#endif
#include "af.h"
#include "isearch.h"
#include "thump.h"

#include "err.h"
#include "opt.h"
#include "rdbms.h"

using namespace std;
namespace io = boost::iostreams;

#define READ_BUFFER_SIZE 4096

#define CONNECTION_BACKLOG 5

#define charset "; charset=utf-8"

extern Thumprq thrq;

struct attribute {
	string name;
	long type;
};

struct attrlist {
	attribute *attr;
	int nattr;
};

void sigchld_handler(int signo)
{
	int status;

	while (wait3(&status, WNOHANG, NULL) >= 0)
		/* do nothing */ ;
}

void sigsegv_handler(int signo)
{
	printf("Segmentation fault\n");
	/* we can write log information here */
	exit(-1);
}

static void install_signal_handlers()
{
	signal(SIGCHLD, sigchld_handler);
	signal(SIGSEGV, sigsegv_handler);
}

static int socket_bind(int sock, uint16_t port)
{
	struct sockaddr_in sin;

	memset(&sin, 0, sizeof sin);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	return ::bind(sock, (struct sockaddr *) &sin, sizeof sin);
}

static int socket_setopt(int sock)
{
	int optval;

	optval = 1;
	return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval,
			  sizeof optval);
}

static int socket_init(uint16_t port)
{
	int sock;

	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		goto socket_init_err;
	if (socket_setopt(sock) < 0)
		goto socket_init_err;
	if (socket_bind(sock, port) < 0)
		goto socket_init_err;
	if (listen(sock, CONNECTION_BACKLOG) < 0)
		goto socket_init_err;
	return sock;

socket_init_err:
/*	return err(ESOCKET);  */
	return -1;
}

/* Maximum number of attributes in a table */
#define MAXATTRLIST (1024)

attrlist *build_attrlist(Rdbms_result *rdbms_result)
{
	attrlist *alist;
	int x;
	int nfields;

	if (rdbms_nfields(rdbms_result, &nfields) != 0)
		return NULL;

	alist = new attrlist;
	alist->nattr = nfields;
	if (alist->nattr)
		alist->attr = new attribute[alist->nattr];
	else
		alist->attr = nullptr;

        for (x = 0; x < alist->nattr; x++) {
		rdbms_fname(rdbms_result, x, &(alist->attr[x].name));
		rdbms_ftype(rdbms_result, x, &(alist->attr[x].type));
	}

	return alist;
}

void free_attrlist(attrlist *alist)
{
	delete [] alist->attr;
	delete alist;
}

void write_datum(const string &datum, ostream &out)
{
	out << datum;

/*
	if (ind == SQL_NULL_DATA)
		fwrite("\\N", 1, 2, out);
*/
}

attrlist *get_attrlist(Rdbms_result *rdbms_result)
{
	attrlist *alist;

	alist = build_attrlist(rdbms_result);
	if (alist == nullptr)
		return nullptr;
	// if (alist->nattr == nullptr) {
	// 	free_attrlist(alist);
	// 	return nullptr;
	// }

	return alist;
}

void write_anvl(ostream &out, Rdbms_result *rdbms_result, int xml)
{
	attrlist *alist;
	int y, c;
	string value;

	out << crlf;

	alist = get_attrlist(rdbms_result);
	if (alist == nullptr)
		return;

	c = 0;
	while (rdbms_fetch(rdbms_result) == 0) {
		if (c > 0)
			out << crlf;
		if (xml)
			out << "<record>" << crlf;
		for (y = 0; y < alist->nattr; y++) {
			if (rdbms_getvalue(rdbms_result, y, &value) == 0) {
				if (xml) {
					out << "  <"
					    << alist->attr[y].name.c_str()
					    << ">";
					write_datum(value, out);
					out << "</"
					    << alist->attr[y].name.c_str()
					    << ">";
				} else {
					out << alist->attr[y].name.c_str()
					    << ":\t";
					write_datum(value, out);
				}
			}
			out << crlf;
		}
		if (xml)
			out << "</record>" << crlf;
		c++;
	}

	free_attrlist(alist);
}

void write_attrlist(const attrlist &alist, ostream &out)
{
	int x;

	for (x = 0; x < alist.nattr; x++) {
		if (x > 0)
			out << '\t';
		out << alist.attr[x].name;
	}
	out << crlf;
}

void write_tab(ostream &out, Rdbms_result *rdbms_result, int header)
{
	attrlist* alist;
	int x;
	string value;

	out << crlf;

	alist = get_attrlist(rdbms_result);
	if (alist == nullptr)
		return;

	if (header)
		write_attrlist(*alist, out);

	while (rdbms_fetch(rdbms_result) == 0) {
		for (x = 0; x < alist->nattr; x++) {
			if (x > 0)
				out << '\t';
			if (rdbms_getvalue(rdbms_result, x, &value) == 0) {
				write_datum(value, out);
			}
		}
		out << crlf;
	}

	free_attrlist(alist);
}

void send_key_file(ostream &out)
{
}

void process_insert(Rdbms_conn *rdbms_conn, ostream &out)
{
}

void process_select(Rdbms_conn *rdbms_conn, ostream &out)
{
	string query;
	Rdbms_result rdbms_result;

	query = "SELECT ";
	if (thrq.unique)
		query += "DISTINCT ";
	if (thrq.show == "")
		query += "*";
	else
		query += thrq.show;
	query += " FROM ";
	if (thrq.in_table != "")
		query += thrq.in_table;
	else
		query += "tables";
	if (thrq.find != "") {
		query += " WHERE (";
		query += thrq.find;
		query += ')';
	}
	if (thrq.sort != "") {
		query += " ORDER BY ";
		query += thrq.sort;
	}
	if (thrq.list_len != "") {
		query += " LIMIT ";
		query += thrq.list_len;
	}
#ifdef DEBUG
	cout << '[' << getpid() << "] " << query << endl;
#endif

	if (rdbms_exec(rdbms_conn, query.c_str(), &rdbms_result) != 0)
		cout << "ERROR: SELECT failed" << endl;

	if (thrq.as == "" || thrq.as == "htab") {
		write_tab(out, &rdbms_result, 1);
	}
	else if (thrq.as == "tab") {
		write_tab(out, &rdbms_result, 0);
	}
	else if (thrq.as == "anvl") {
		write_anvl(out, &rdbms_result, 0);
	}
	else if (thrq.as == "xml") {
		write_anvl(out, &rdbms_result, 1);
	}
	rdbms_close_result(&rdbms_result);
}

#define THUMP_CONF "/etc/thump"

#define CONF_DB_TYPE_RELATIONAL 1
#define CONF_DB_TYPE_ISEARCH 2

int file_exists(const string &fn)
{
	return access(fn.c_str(), F_OK) == 0;
}

int conf_db_type(const string &db)
{
	string dir;
	int e;

	// dir = THUMP_CONF;
	// dir += "/db/";
	// dir += db;
	// dir += "/type/xdb";
	// e = file_exists(dir);

	// if (e)
	// 	return CONF_DB_TYPE_ISEARCH;
	// else
	// 	return CONF_DB_TYPE_RELATIONAL;

        return CONF_DB_TYPE_ISEARCH;
}

int conf_db_rdbms_driver(const string &db)
{
	string dir;
	int e;

	dir = THUMP_CONF;
	dir += "/db/";
	dir += db;
	dir += "/rdbms/driver/odbc";
	e = file_exists(dir);

	if (e)
		return RDBMS_DRIVER_ODBC;
	else
		return RDBMS_DRIVER_POSTGRES;
}

void status_code(const char *status, int code, ostream &out)
{
	out << "x_thump_status\tx_thump_code" << crlf;
	out << status << '\t' << code << crlf;
}

int err_status_code(const char *status, int code, ostream &out)
{
	status_code(status, code, out);
	return -1;
}

int process(ostream &out)
{
	Rdbms_conn rdbms_conn;
	int r;
	int rdbms_driver;

	write_header(out);

	if (thrq.help) {
		out << "Content-Type: text/html" << charset << crlf << crlf;
		out << "(Help text goes here)" << crlf;
			  return 0;
	}

	if (thrq.key != "") {
		send_key_file(out);
		return 0;
	}

	if (thrq.in_db == "")
		return 0;

	out << "Content-Type: text/plain" << charset << crlf;

	switch (conf_db_type(thrq.in_db)) {
	case CONF_DB_TYPE_RELATIONAL:
		break;
	case CONF_DB_TYPE_ISEARCH:
		out << crlf;
		af_query(out, &thrq);
		break;
	default:
		out << crlf;
		return err_status_code("Specified database is not present in "
				       "server configuration", 500, out);
	}

//	if (conf_db_type(ests(thrq.in_db)) != CONF_DB_TYPE_RELATIONAL) {
//		write_crlf(out);
//		return err_status_code("Specified database is not present in "
//				       "server configuration", 500, out);
//	}

/*
	if (!thrq.user_name)
		conf_db_user_default(ests(thrq.in_db));
*/
	if (thrq.user_name == "") {
		thrq.user_name = "USERNAME";
		thrq.user_auth = "PASSWORD";
	}

	rdbms_driver = conf_db_rdbms_driver(thrq.in_db);
/*
	if (thrq.append_attr || thrq.replace)
		r = rdbms_connectdb(RDBMS_TYPE_ODBC, ests(thrq.in_db),
				    ests(thrq.user_name), ests(thrq.user_auth),
				    &rdbms_conn);
	else
		r = rdbms_connectdb(RDBMS_TYPE_POSTGRES, ests(thrq.in_db),
				    ests(thrq.user_name), ests(thrq.user_auth),
				    &rdbms_conn);
*/
	r = rdbms_connectdb(rdbms_driver, thrq.in_db,
			    thrq.user_name, thrq.user_auth,
			    &rdbms_conn);

	if (r != 0) {
		out << crlf << "user(): unable to authenticate" << crlf;
#ifdef DEBUG
		cout << '[' << getpid() << "] RDBMS connection failed" << endl;
#endif
		exit(-1);
	}

	if (thrq.append_attr != "" || thrq.replace != "")
		process_insert(&rdbms_conn, out);
	else
		process_select(&rdbms_conn, out);

	rdbms_closedb(&rdbms_conn);

	return 0;
}

int connect_debug(int sock)
{
	int n;
	char buf[2];

	while ( (n = read(sock, buf, 1)) ) {
		if (n < 0)
			return -1;
		printf("%c", buf[0]);
		fflush(stdout);
	}
	return 0;
}

static int server_connect(FILE *in, FILE *out)
{
	yycompile(in, out);

#ifdef OLD_BOOST
	io::file_descriptor_sink fds(fileno(out));
#else
	io::file_descriptor_sink fds(fileno(out), io::never_close_handle);
#endif
	io::stream_buffer<io::file_descriptor_sink> outb(fds);
	ostream outs(&outb);

	process(outs);

	return 0;
}

static int socket_connect(int sock)
{
	FILE *file;

	file = fdopen(sock, "r+b");
	if (!file) {
		printf("Error opening socket as stream\n");
		return -1;
	}

	server_connect(file, file);

	fclose(file);

	return 0;
}

static int server_start(int listen_port)
{
	int main_socket;
	int child_socket;
	struct sockaddr_in addr;
	int addr_size = sizeof addr;
	pid_t f;

	if ((main_socket = socket_init(listen_port)) < 0)
		return -1;
	install_signal_handlers();
#ifdef DEBUG
	printf("*** Server started ***\n");
#endif
	while (1) {
		if ((child_socket = accept(main_socket,
					   (struct sockaddr *) &addr,
					   (socklen_t *) &addr_size)) < 0) {
			if (errno == EINTR)
				continue;
			else 
				return -1;
		}
		if ((f = fork()) < 0)
			return -1;
		if (f == 0) {  /* child */
			int e;
			close(main_socket);
#ifdef DEBUG
			printf("[%i] *** Connected ***\n",
			       getpid());
#endif
/*			if ((e = connect_debug(child_socket)) < 0)*/
			if ((e = socket_connect(child_socket)) < 0)
				printf("Child exiting abnormally (%i)\n", e);
#ifdef DEBUG
			printf("[%i] Connection closed normally\n",
			       getpid());
#endif
			exit(e);
		} else {  /* parent; f == child's PID */
			close(child_socket);
		}
	}
}

static int help()
{
	fprintf(stderr, "--help\tPrint this help text\n");
	fprintf(stderr, "-I\tInteractive mode\n");
	fprintf(stderr, "-p [x]\tListen on port x\n");
	return 0;
}

int main(int argc, char *argv[])
{
	Opt opt;

	prgname = argv[0];

	if (evalopt(argc, argv, &opt) < 0)
		return -1;

	if (opt.help)
		return help();

	if (opt.interactive) {
		server_connect(stdin, stdout);
		return 0;
	}

	if (opt.port == 0)
		opt.port = 8660;
	if (server_start(opt.port) < 0) {
		fprintf(stderr, "%s: unable to listen on port %i\n", prgname,
			opt.port);
		return -1;
	}

	return 0;
}
