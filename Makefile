.PHONY: ioquake3 plugin web

all: ioquake3 plugin web

ioquake3:
	$(MAKE) -C ioquake3

plugin:
	$(MAKE) -C firebreath/build/projects/q3plugin

web:
	$(MAKE) -C web