
ERPCPLATFORM := Linux
ERPCLIB := tools/erpc/Release/$(ERPCPLATFORM)/erpc/lib/liberpc.a
ERPCGEN := tools/erpc/Release/$(ERPCPLATFORM)/erpcgen/erpcgen
ERPCSNI := tools/erpc/Release/$(ERPCPLATFORM)/erpcsniffer/erpcsniffer

erpc/dma.h: src/dma/dma.erpc
	tools/erpc/Release/Linux/erpcgen/erpcgen -o erpc $^

$(ERPCLIB): submodules_erpc

submodules_erpc: submodules
	cd tools/erpc && make -j`nproc`

submodules:
	git submodule update --init

.PHONY: submodules submodules_erpc
