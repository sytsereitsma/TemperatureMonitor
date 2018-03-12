PHONY: firsttime all show clean compile test native

APPNAME=temperature-monitor

#http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/#combine
DEPFLAGS = -MT $@ -MMD -MP -MF $(OBJ_DIR)/$*.Td
POSTCOMPILE = @mv -f $(OBJ_DIR)/$*.Td $(OBJ_DIR)/$*.d && touch $@
COMPILE = @$(GPP) $(DEPFLAGS) $(CPP_FLAGS) $(addprefix -I, $(INCLUDES)) -c -o $@ $<
CPP_FLAGS:=-std=c++11 -x c++ -Wall -O0 -g3
LD_FLAGS:=

ifeq ($(ARDUINO),1)
    $(info "TODO Arduino build")
else
    $(info "Native build")
    BLD_DIR:=bld/native
    OBJ_DIR:=$(BLD_DIR)/obj
	GPP:=g++
	INCLUDES+=\
	    $(EXTERNAL_DIR)/ArduinoEmulator/src\
		$(EXTERNAL_DIR)/Catch/single_include \
		$(EXTERNAL_DIR)/FakeIt/single_header/catch\
		src/native

	CPP_FILES+= $(wildcard $(EXTERNAL_DIR)/ArduinoEmulator/src/*.cpp)

	CPP_FLAGS+= -DF_CPU=8000000
	CPP_FLAGS+= -include "Serial.h"\
	            -include "cstdlib" \
	            -include "cstring" \
	            -include "cctype" \
	            -include "cstdio"
	LD_FLAGS+=-pthread
endif

EXTERNAL_DIR=$(BLD_DIR)/external
TARGET:=$(OBJ_DIR)/$(APPNAME)

#Clone git repository and checkout optional tag:
#Example (checkout tag):
#	$(call git_clone,https://github.com/eranpeer/FakeIt.git,outdir,tags/v1.2.3)
#or (stay on master):
#	$(call git_clone,https://github.com/eranpeer/FakeIt.git,outdir)
git_clone = @git clone $1 $2 && [ ! -n "$3" ] || (cd $2 && git checkout $3)

$(EXTERNAL_DIR)/FakeIt:
	$(call git_clone,https://github.com/eranpeer/FakeIt.git,$@,tags/2.0.4)

$(EXTERNAL_DIR)/Catch:
	#fakeit 2.04 does not work with the latest (v2.2.0) 
	$(call git_clone,https://github.com/catchorg/Catch2.git,$@,tags/v2.0.1)

$(EXTERNAL_DIR)/ArduinoEmulator:
	$(call git_clone,https://github.com/sytsereitsma/ArduinoEmulator.git,$@)

$(EXTERNAL_DIR)/OneWire:
	$(call git_clone,https://github.com/PaulStoffregen/OneWire.git,$@,tags/v2.3.3)

EXTERNALS=\
	$(EXTERNAL_DIR)/FakeIt\
	$(EXTERNAL_DIR)/Catch\
	$(EXTERNAL_DIR)/ArduinoEmulator\
	$(EXTERNAL_DIR)/OneWire

firsttime: $(EXTERNALS)

INCLUDES += \
	src

ifeq ($(findstring test,$(MAKECMDGOALS)),test)
	IS_TEST_BUILD=yes
else ifeq ($(findstring native,$(MAKECMDGOALS)),native)
	IS_NATIVE_BUILD=yes
endif

ifeq ($(IS_TEST_BUILD),yes)
	EXCLUDED_SOURCES+=src/native/nativemain.cpp
	CPP_FILES+=$(wildcard test/*.cpp)
else ifeq ("$(IS_NATIVE_BUILD)","yes")
	EXCLUDED_SOURCES+=test/testmain.cpp
else

endif

CPP_FILES += $(wildcard src/*.cpp)
CPP_FILES += $(wildcard src/native/*.cpp)
CPP_FILES += src/TemperatureMonitor.ino
FILTERED_CPP_FILES=$(filter-out $(EXCLUDED_SOURCES),$(CPP_FILES))
OBJ_FILES += $(patsubst %.ino, $(OBJ_DIR)/%.o, $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(FILTERED_CPP_FILES))))

$(OBJ_DIR):
	@mkdir -p $@

compile: $(OBJ_FILES) |$(OBJ_DIR)

$(TARGET): compile
	@echo "=> Linking $@"
	@$(GPP) $(LD_FLAGS) -o $@ $(OBJ_FILES)

$(OBJ_DIR)/%.o: src/%.cpp $(OBJ_DIR)/%.d |$(OBJ_DIR)
	@echo "=> $<"
	$(COMPILE)
	$(POSTCOMPILE)

$(OBJ_DIR)/%.o: src/native/%.cpp $(OBJ_DIR)/%.d |$(OBJ_DIR)
	@echo "=> $<"
	$(COMPILE)
	$(POSTCOMPILE)

$(OBJ_DIR)/%.o: src/%.ino $(OBJ_DIR)/%.d |$(OBJ_DIR)
	@echo "=> $<"
	$(COMPILE)
	$(POSTCOMPILE)

$(OBJ_DIR)/%.o: $(EXTERNAL_DIR)/ArduinoEmulator/src/%.cpp $(OBJ_DIR)/%.d |$(OBJ_DIR)
	@echo "=> $<"
	$(COMPILE)
	$(POSTCOMPILE)

$(OBJ_DIR)/%.o: test/%.cpp |$(OBJ_DIR)
	@echo "=> $<"
	$(COMPILE)
	$(POSTCOMPILE)

$(OBJ_DIR)/%.d: ;
.PRECIOUS: $(OBJ_DIR)/%.d
include $(wildcard $(patsubst %,$(OBJ_DIR)/%.d, $(basename $(FILTERED_CPP_FILES))))

$(OBJ_DIR)/.cleanMake: Makefile |$(OBJ_DIR)
	@rm -rf $(OBJ_DIR)/*
	touch $@


all: firsttime $(OBJ_DIR)/.cleanMake $(TARGET)

clean: 
	@rm -rf $(OBJ_DIR)

native: all

test: all
	@echo
	@echo "*****************************************************"
	@echo "Running unit tests"
	@$(TARGET)

show:
	$(info bb $(wildcard $(patsubst %,$(OBJ_DIR)/%.d, $(basename $(FILTERED_CPP_FILES)))))
	@echo "*** CPP_FLAGS *** "
	@echo $(CPP_FLAGS)
	@echo

	@echo "*** INCLUDES *** "
	@echo $(INCLUDES)
	@echo

	@echo "*** EXCLUDED SOURCES *** "
	@echo $(EXCLUDED_SOURCES)
	@echo

	@echo "*** SOURCES *** "
	@echo $(FILTERED_CPP_FILES)
	@echo

	@echo "*** OBJECTS *** "
	@echo $(OBJ_FILES)
	@echo

