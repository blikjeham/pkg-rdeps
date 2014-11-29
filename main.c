#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pkg.h>

struct dep {
	const char *name;
	struct dep *parent;
	struct dep *child;
	struct dep *next;
};

static void usage(const char *progname)
{
	printf("Usage:\n%s pkgname\n", progname);
	exit(255);
}

struct pkg *find_pkg(struct pkgdb *db, const char *pkgname)
{
	int ret;
	struct pkg *pkg = NULL;
	struct pkgdb_it *it;
	if ((it = pkgdb_query(db, pkgname, MATCH_GLOB)) == NULL) {
		printf("Could not find pkg with name: %s\n", pkgname);
		return NULL;
	}

	while ((ret = pkgdb_it_next(it, &pkg, PKG_LOAD_RDEPS)) == EPKG_OK) {
		return pkg;
	}
	return NULL;
}

static int get_rdeps(int level, struct pkgdb *db, const char *pkgname, struct dep *rdep)
{
	const char *name;
	struct pkg_dep *req = NULL;
	struct pkg *pkg;
	struct dep dep;
	int indent;
	int depcount = 0;

	if ((pkg = find_pkg(db, pkgname)) == NULL) {
		printf("Could not find package with name: %s\n", pkgname);
		return -1;
	}

	while (pkg_rdeps(pkg, &req) == EPKG_OK) {
		dep.name = pkg_dep_name(req);
		dep.parent = rdep;
		indent = level;
		while (indent > 0) {
			printf("-");
			indent--;
		}
		printf("> %s\n", dep.name);
		depcount++;
		depcount += get_rdeps(level + 1, db, dep.name, &dep);
	}
	return depcount;
}

int main(int argc, char **argv)
{
	struct pkgdb *db = NULL;
	struct pkgdb_it *it = NULL;
	struct pkg *pkg = NULL;
	struct dep rdep;
	char *conffile = NULL;
	char *reposdir = NULL;
	int ret;
	char *pkgname;
	const char *name;

	pkg_init(conffile, reposdir);

	if (argc > 1) {
		pkgname = argv[1];
	} else {
		usage(argv[0]);
	}
	ret = pkgdb_access(PKGDB_MODE_READ, PKGDB_DB_LOCAL);
	ret = pkgdb_open(&db, PKGDB_DEFAULT);
	if (pkgdb_obtain_lock(db, PKGDB_LOCK_READONLY) != EPKG_OK) {
		pkgdb_close(db);
		printf("Failed to get lock\n");
	}

	rdep.name = pkgname;
	if (!get_rdeps(1, db, pkgname, &rdep))
		printf("Nothing depends on %s\n", pkgname);

	pkgdb_it_free(it);
	pkgdb_release_lock(db, PKGDB_LOCK_READONLY);
	pkgdb_close(db);
	return 0;
}
