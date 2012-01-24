.PHONY: shim plugin

all: plugin shim

plugin: shim
	$(MAKE) -C firebreath/build/projects/q3plugin

shim:
	$(MAKE) -C shim