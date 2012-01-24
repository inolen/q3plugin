.PHONY: shim plugin

plugin: shim
	$(MAKE) -C firebreath/build/projects/q3plugin

shim:
	$(MAKE) -C shim