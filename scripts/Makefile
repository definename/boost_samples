.PHONY: all clean

BUILD_DIR = ../build

all: debug

debug:
	mkdir -p $(BUILD_DIR)/gcc_debug
	cd $(BUILD_DIR)/gcc_debug && cmake ../.. -DCMAKE_BUILD_TYPE=Debug && cmake --build .

release:
	mkdir -p $(BUILD_DIR)/gcc_release
	cd $(BUILD_DIR)/gcc_release && cmake ../.. -DCMAKE_BUILD_TYPE=Release && cmake --build .

clean:
	rm -rf $(BUILD_DIR)
