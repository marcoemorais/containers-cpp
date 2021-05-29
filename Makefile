PWD=$(shell pwd)
NAME=$(shell basename $(PWD))
IMAGE=$(NAME)
TAG=local
ACTION?=test

DIRS := \
	01-list \
	02-stack \
	03-queue \
	04-deque \
	05-tree

.PHONY: $(DIRS) all-clean

all:: $(DIRS)

$(DIRS):
	$(MAKE) -C $@ $(ACTION)

all-clean:
	$(MAKE) ACTION=clean

include Makefile.defs
