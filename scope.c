#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "util.h"
#include "htab.h"
#include "scope.h"

struct scope filescope;

struct scope *
mkscope(struct scope *parent)
{
	struct scope *s;

	s = xmalloc(sizeof(*s));
	s->decls = NULL;
	s->tags = NULL;
	s->breaklabel = parent->breaklabel;
	s->continuelabel = parent->continuelabel;
	s->switchcases = parent->switchcases;
	s->parent = parent;

	return s;
}

struct scope *
delscope(struct scope *s)
{
	struct scope *parent = s->parent;

	if (s->decls)
		delhtab(s->decls, NULL);
	if (s->tags)
		delhtab(s->tags, NULL);
	free(s);

	return parent;
}

struct declaration *
scopegetdecl(struct scope *s, const char *name, bool recurse)
{
	struct declaration *d;
	struct hashtablekey k;

	htabstrkey(&k, name);
	do {
		d = s->decls ? htabget(s->decls, &k) : NULL;
		s = s->parent;
	} while (!d && s && recurse);

	return d;
}

struct type *
scopegettag(struct scope *s, const char *name, bool recurse)
{
	struct type *t;
	struct hashtablekey k;

	htabstrkey(&k, name);
	do {
		t = s->tags ? htabget(s->tags, &k) : NULL;
		s = s->parent;
	} while (!t && s && recurse);

	return t;
}

void
scopeputdecl(struct scope *s, const char *name, struct declaration *d)
{
	struct hashtablekey k;

	if (!s->decls)
		s->decls = mkhtab(32);
	htabstrkey(&k, name);
	*htabput(s->decls, &k) = d;
}

void
scopeputtag(struct scope *s, const char *name, struct type *t)
{
	struct hashtablekey k;

	if (!s->tags)
		s->tags = mkhtab(32);
	htabstrkey(&k, name);
	*htabput(s->tags, &k) = t;
}