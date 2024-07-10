.PHONY: all clean
nn_framework?=tflm
debug?=0
# default action: build all
all:
	cd	CM4 && $(MAKE) nn_framework=$(nn_framework) debug=$(debug)
	cd	CM7 && $(MAKE) nn_framework=$(nn_framework) debug=$(debug)

clean:
	cd CM4 && $(MAKE) clean
	cd CM7 && $(MAKE) clean

# *** EOF ***