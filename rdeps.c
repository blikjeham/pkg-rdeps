#include <assert.h>
#include <stdio.h>
#include <sysexits.h>
#include <unistd.h>
#include <inttypes.h>
#include <libutil.h>

#include <pkg.h>

static char myname[] = "rdeps";
static char version[] = "1.0.0";
static char plugdesc[] = "Show the reverse dependency tree";

struct pkgdb_it *it;

static int plugin_mystats_usage(void);

int pkg_plugin_init(struct pkg_plugin *p)
{
	pkg_plugin_set(p, PKG_PLUGIN_NAME, myname);
	pkg_plugin_set(p, PKG_PLUGIN_DESC, plugdesc);
	pkg_plugin_set(p, PKG_PLUGIN_VERSION, version);

	return EPKG_OK;
}

int pkg_plugin_shutdown(void)
{
	/* Nothing to do here */
	return EPKG_OK;
}

static int plugin_mystats_usage(void)
{
	fprintf(stderr, "usage: pkg %s <pkg-name>\n\n", myname);
	fprintf(stderr, "%s\n", plugdesc);
	return EX_USAGE;
}

static void print_indent(uint level, char c)
{
	while (level--) {
		putchar(c);
	}
}

static struct pkg *find_pkg(struct pkgdb *db, const char *pkgname)
{
	struct pkg *pkg = NULL;
	if ((it = pkgdb_query(db, pkgname, MATCH_GLOB)) == NULL) {
		return NULL;
	}

	while (pkgdb_it_next(it, &pkg, PKG_LOAD_RDEPS) == EPKG_OK) {
		return pkg;
	}
	return NULL;
}

static int get_rdeps(uint level, struct pkgdb *db, const char *pkgname)
{
	struct pkg *pkg;
	struct pkg_dep *req = NULL;
	const char *name;
	int count = 0;

	if ((pkg = find_pkg(db, pkgname)) == NULL) {
		printf("No package found: %s\n", pkgname);
		return -1;
	}

	while (pkg_rdeps(pkg, &req) == EPKG_OK) {
		name = pkg_dep_name(req);
		print_indent(level, '-');
		printf("> %s\n", name);
		count++;
		count += get_rdeps(level + 1, db, name);
	}

	return count;
}

static int plugin_rdeps_callback(int argc, char **argv)
{
	struct pkgdb *db = NULL;
	char *pkgname;
	int ret;

	if (argc != 2)
		return plugin_mystats_usage();

	pkgname = argv[1];

	if (pkgdb_open(&db, PKGDB_DEFAULT) != EPKG_OK) {
		return EX_IOERR;
	}

	if ((ret = get_rdeps(1, db, pkgname)) == 0)
		printf("No reverse dependencies found for %s\n", pkgname);

	if (ret < 0)
		return EPKG_FATAL;

	pkgdb_it_free(it);
	pkgdb_close(db);
	return EPKG_OK;
}

int pkg_register_cmd(int i, const char **name, const char **desc,
		     int (**exec)(int argc, char **argv))
{
	*name = myname;
	*desc = plugdesc;
	*exec = plugin_rdeps_callback;

	return (EPKG_OK);
}

int pkg_register_cmd_count(void)
{
	return 1;
}
