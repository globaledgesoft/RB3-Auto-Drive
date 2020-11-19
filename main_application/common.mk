#==============================================================================
# Common Rules
#
# These variables need to be filled by the caller:
#   CXX_SRC
#   C_SRC
#   TARGET_OBJ
#   TARGETS
#   FILES
#

FILE_LIST := files.txt
COUNT := ./make/count.sh
MK := $(word 1,$(MAKEFILE_LIST))
ME := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

OBJ=$(CXX_SRC:.cpp=.o) $(C_SRC:.c=.o)
DEP=$(OBJ:.o=.d) $(TARGET_OBJ:.o=.d)

GET_VER   = ./scripts/version.sh
CFLAGS   += -D_CS_VERSION_="\"$(shell [ -x $(GET_VER) ] && $(GET_VER) || echo 0000000)\""
CXXFLAGS += -std=c++11

.PHONY: all clean distclean

all: $(TARGETS)

clean:
	rm -f $(TARGETS) $(FILE_LIST)
	find . -name "*.o" -delete
	find . -name "*.d" -delete

distclean:
	rm -f $(TARGETS) $(FILE_LIST)
	find . -name "*.o" -delete
	find . -name "*.d" -delete

-include $(DEP)

%.o: %.c $(MK) $(ME)
	@[ -f $(COUNT) ] && $(COUNT) $(FILE_LIST) $^ || true
	@$(CC) -c $< -MM -MT $@ -MF $(@:.o=.d) $(CFLAGS)
	$(CC) -c $< $(CFLAGS) -o $@

%.o: %.cpp $(MK) $(ME)
	@[ -f $(COUNT) ] && $(COUNT) $(FILE_LIST) $^ || true
	@$(CXX) -c $< -MM -MT $@ -MF $(@:.o=.d) $(CXXFLAGS) $(CFLAGS)
	$(CXX) -c $< $(CXXFLAGS) $(CFLAGS) -o $@

$(TARGETS): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(CFLAGS) $(LDFLAGS)
	@[ -f $(COUNT) -a -n "$(FILES)" ] && $(COUNT) $(FILE_LIST) $(FILES) || true
	@[ -f $(COUNT) ] && $(COUNT) $(FILE_LIST) || true

