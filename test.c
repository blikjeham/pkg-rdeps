#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <pkg.h>

#include "ucl.h"

int main(int argc, char **argv)
{
	struct pkg_plugin *p = NULL;
	char pluginfile[MAXPATHLEN];
	const ucl_object_t *obj, *cur;
	ucl_object_iter_t it = NULL;
	const char *plugdir;
	char *conffile = NULL;
	char *reposdir = NULL;
	bool plug_enabled = false;
	int (*init_func)(struct pkg_plugin *);
	int ret;

	pkg_init(conffile, reposdir);
	plug_enabled = pkg_object_bool(pkg_config_get("PKG_ENABLE_PLUGINS"));
	printf("%d\n", plug_enabled);
	plugdir = pkg_object_string(pkg_config_get("PKG_PLUGINS_DIR"));
	printf("%s\n", plugdir);

	obj = pkg_config_get("PLUGINS");
	errx()
	return 0;
}
