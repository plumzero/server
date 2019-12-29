

.PHONY: all
all:
	$(MAKE) -C dataparse
	$(MAKE) -C manager
	$(MAKE) -C message
	$(MAKE) -C support
	$(MAKE) -C wrapper
	$(MAKE) -C simpletest
	$(MAKE) -C servergateway
	$(MAKE) -C serverforward
	$(MAKE) -C serverdatabase
	$(MAKE) -C serverbusiness

.PHONY: clean
clean:
	$(MAKE) -C dataparse clean
	$(MAKE) -C manager clean
	$(MAKE) -C message clean
	$(MAKE) -C support clean
	$(MAKE) -C wrapper clean
	$(MAKE) -C simpletest clean
	$(MAKE) -C servergateway clean
	$(MAKE) -C serverforward clean
	$(MAKE) -C serverdatabase clean
	$(MAKE) -C serverbusiness clean