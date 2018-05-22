/* See LICENSE file for copyright and license details. */
#include <X11/XKBlib.h> 
#include <X11/Xlib.h>

#include "../util.h"
#include <stdio.h>
#include <string.h>

#define VARIANT_MAX 256
#define LAYOUT_MAX 4

/* Given a token (sym) from the xkb_symbols string
 * check whether it is a valid layout/variant. The
 * EXCLUDES array contains invalid layouts/variants
 * that are part of the xkb rules.
 */
int
IsLayoutOrVariant(char *sym)
{
	static const char* EXCLUDES[] = { "evdev", "inet", "pc", "base" };

	size_t i;
	for (i = 0; i < sizeof(EXCLUDES)/sizeof(EXCLUDES[0]); ++i)
		if (strstr(sym, EXCLUDES[i]))
			return 0;

	return 1;
}

/* Given a xkb_symbols string (syms) retrieve
 * and populate the provided layout and variant
 * strings
 */
void
GetLayoutAndVariant(char *syms, char layout[], char variant[])
{
	char *token,
		 *paren,
		 *copy,
		 *delims = "+:";

	copy = strdup(syms);
	token = strtok(copy, delims);
	while (token != NULL && (strlen(layout) == 0 || strlen(variant) == 0)) {
		paren = strchr(token, '(');
		if (paren && IsLayoutOrVariant(paren + 1)) {
			strncpy (variant, paren, VARIANT_MAX);
		}

		if (IsLayoutOrVariant(token)) {
			strncpy (layout, token, LAYOUT_MAX);
		}

		token = strtok(NULL,delims);
	}
}

const char *
keymap(void)
{
	Display *dpy;
	char *symbols = NULL;
	char layout[LAYOUT_MAX];
	char variant[VARIANT_MAX];

	memset(layout, '\0', LAYOUT_MAX);
	memset(variant, '\0', VARIANT_MAX);

	if (!(dpy = XOpenDisplay(NULL))) {
		warn("XOpenDisplay: Failed to open display");
		return NULL;
	}

	XkbDescRec* desc = XkbAllocKeyboard();
	if (!desc) {
		warn("XkbGetNames: failed to get XkbSymbolsNameMask keyboard component");
		XCloseDisplay(dpy);
		return NULL;
	}


	XkbGetNames(dpy, XkbSymbolsNameMask | XkbGroupNamesMask, desc);
	if (desc->names) {

//--	XkbStateRec state;
//--	XkbGetState(dpy, XkbUseCoreKbd, &state);
//--	char *group = XGetAtomName(dpy, desc->names->groups[state.group]);
//--	printf("Full name: %s\n", group);

		symbols = XGetAtomName(dpy, desc->names->symbols);
		GetLayoutAndVariant(symbols, layout, variant);
		printf("symbols: %s\n", symbols);
		printf( "%s %s\n", layout, variant);
		XFree(symbols);
	} else {
		warn("XkbGetNames: failed to retrieve symbols for keys");
		return NULL;
	}

	XkbFreeKeyboard(desc, XkbSymbolsNameMask | XkbGroupNamesMask, 1);
	XCloseDisplay(dpy);
	return "";
}
