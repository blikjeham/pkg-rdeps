#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pkg.h>

unsigned flags;

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

	while ((ret = pkgdb_it_next(it, &pkg, flags)) == EPKG_OK) {
		return pkg;
	}
	return NULL;
}

static int get_deps(int level, int (*depper)(struct pkg *, struct pkg_dep **), struct pkgdb *db, const char *pkgname)
{
	const char *name;
	struct pkg_dep *req = NULL;
	struct pkg *pkg;
	int indent;
	int depcount = 0;

	if ((pkg = find_pkg(db, pkgname)) == NULL) {
		printf("\nCould not find package with name: %s\n", pkgname);
		return -1;
	}

	while (depper(pkg, &req) == EPKG_OK) {
		name = pkg_dep_name(req);
		indent = level;
		printf("\n");
		while (indent > 0) {
			printf("-");
			indent--;
		}
		printf("> %s", name);
		depcount++;
		depcount += get_deps(level + 1, depper, db, name);
	}
	return depcount;
}

const char *parse_args(int argc, char **argv)
{
	int i = 1;
	while (i < argc) {
		if (!strncmp(argv[i], "-r", 2)) {
			flags = PKG_LOAD_RDEPS;
		} else {
			return argv[i];
		}
		i++;
	}
	usage(argv[0]);
}

int main(int argc, char **argv)
{
	struct pkgdb *db = NULL;
	struct pkgdb_it *it = NULL;
	struct pkg *pkg = NULL;
	char *conffile = NULL;
	char *reposdir = NULL;
	int ret;
	const char *pkgname;

	flags = PKG_LOAD_DEPS;

	pkg_init(conffile, reposdir);

	pkgname = parse_args(argc, argv);

	ret = pkgdb_access(PKGDB_MODE_READ, PKGDB_DB_LOCAL);
	ret = pkgdb_open(&db, PKGDB_DEFAULT);
	if (pkgdb_obtain_lock(db, PKGDB_LOCK_READONLY) != EPKG_OK) {
		pkgdb_close(db);
		printf("Failed to get lock\n");
	}

	printf("%s", pkgname);
	if (flags == PKG_LOAD_RDEPS) {
		if (!get_deps(1, &pkg_rdeps, db, pkgname))
			printf(" has no dependencies", pkgname);
	} else {
		if (!get_deps(1, &pkg_deps, db, pkgname))
			printf(" depends on nothing", pkgname);
	}
	printf("\n");

	pkgdb_it_free(it);
	pkgdb_release_lock(db, PKGDB_LOCK_READONLY);
	pkgdb_close(db);
	return 0;
}
