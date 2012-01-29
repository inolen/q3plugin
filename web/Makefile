.PHONY: jslint

all: jslint

jslint:
	 find -name "*.js" | grep -v "node_modules/\|backbone\|jquery\|underscore\|jade\|bootstrap" | xargs jslint --indent=2 --nomen=true --vars=true