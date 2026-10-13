#!/system/bin/sh
SKIPUNZIP=1

ui_print "- Extracting module files"
unzip -o "$ZIPFILE" -x 'META-INF/*' -d $MODPATH >&2

ui_print "- Setting permissions"
set_perm_recursive $MODPATH 0 0 0755 0644

ui_print "- Installation complete"