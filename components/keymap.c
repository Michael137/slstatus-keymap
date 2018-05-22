/* See LICENSE file for copyright and license details. */
#include <X11/XKBlib.h> 
#include <X11/Xlib.h>

#include "../util.h"
#include <stdio.h>

const char *
keymap(void)
{
	Display *dpy;
	char *keymap = NULL;

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

	XkbGetNames(dpy, XkbSymbolsNameMask, desc);
	if (desc->names) {
		keymap = XGetAtomName(dpy, desc->names->symbols);
		// TODO: Copy and XFree keymap
		
	} else {
		warn("XkbGetNames: failed to retrieve symbols for keys");
		return NULL;
	}

	XkbFreeKeyboard(desc, XkbSymbolsNameMask, 1);
	XCloseDisplay(dpy);
	return keymap;
}
