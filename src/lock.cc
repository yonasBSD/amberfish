int etymon_db_ready(char* dbname, ETYMON_LOG* log) {
	char fn[ETYMON_MAX_PATH_SIZE];
	int lock_fd;

	etymon_db_construct_path(ETYMON_DBF_LOCK, dbname, fn);
	lock_fd = open(fn, O_RDONLY | ETYMON_AF_O_LARGEFILE, ETYMON_DB_PERM);
	if (lock_fd == -1) {
		return 1;
	} else {
		close(lock_fd);
		return 0;
	}
}


void etymon_db_unlock(char* dbname, ETYMON_LOG* log) {
	char fn[ETYMON_MAX_PATH_SIZE];

	etymon_db_construct_path(ETYMON_DBF_LOCK, dbname, fn);
	unlink(fn);
}


/* returns 1 if lock was obtained */
int etymon_db_lock(char* dbname, ETYMON_LOG* log) {
	char fn[ETYMON_MAX_PATH_SIZE];
	int lock_fd;
	
	etymon_db_construct_path(ETYMON_DBF_LOCK, dbname, fn);
	lock_fd = open(fn, O_RDONLY | ETYMON_AF_O_LARGEFILE, ETYMON_DB_PERM);
	if (lock_fd != -1) {
		int e;
		char s[ETYMON_MAX_MSG_SIZE];
		sprintf(s, "%s: Database not ready", dbname);
		e = log->error(s, 1);
		close(lock_fd);
		if (e != 0) {
			exit(e);
		}
		return 0;
	}
	lock_fd = open(fn, O_WRONLY | O_CREAT | O_TRUNC | ETYMON_AF_O_LARGEFILE, ETYMON_DB_PERM);
	if (lock_fd == -1) {
		int e;
		char s[ETYMON_MAX_MSG_SIZE];
		sprintf(s, "%s: Error writing to database", dbname);
		e = log->error(s, 1);
		if (e != 0) {
			exit(e);
		}
		return 0;
	}
	close(lock_fd);
	return 1;
}
