.PHONY: jslint

all: jslint

jslint:
	 find -name "*.js" | grep -v "node_modules/\|support" | xargs jslint --indent=2 --nomen=true --vars=true --regexp=true