/* See LICENSE file for copyright and license details. */
#include <X11/XKBlib.h> 
#include <X11/Xlib.h>

#include "../util.h"
#include <stdio.h>

const char *
keymap(void)
{
	Display *dpy;
	if (!(dpy = XOpenDisplay(NULL))) {
		warn("XOpenDisplay: Failed to open display");
		return NULL;
	}

	XkbStateRec state;
	if (XkbGetState(dpy, XkbUseCoreKbd, &state) != 0) {
		warn("XkbGetState: failed to get keyboard state");
		return NULL;
	};

	XkbDescRec* desc = XkbGetMap(dpy, XkbNamesMask, XkbUseCoreKbd);
	if (!desc)
		warn("XkbGetMap: failed to get XkbNamesMask keyboard component");

	XkbGetNames(dpy, XkbSymbolsNameMask, desc);
	if (desc->names) {
		char *atom = XGetAtomName(dpy, desc->names->symbols);
		XkbFreeClientMap(desc, XkbNamesMask, 1);
		return atom;
	} else {
		warn("XkbGetNames: failed to retrieve symbols for keys");
	}
	
	XkbFreeClientMap(desc, XkbNamesMask, 1);
	return NULL;
}
