GIT_VERSION = $$system(git describe --always --abbrev=0 --tags)

GIT_VERSION ~= s/-/"."
GIT_VERSION ~= s/[a-zA-Z]/""

DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"
