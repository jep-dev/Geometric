# Do not edit this file directly. Configuration is done in Makefile.conf.
# Most configuration here honors existing settings.
include Makefile.conf

DEST_DIR:=
EMPTY:=
SPACE:=$(EMPTY) $(EMPTY)
define BREAK
$1
$(1:%=  )  
endef
BR:=$(call BREAK,\)#) <- Backslashes can confuse highlighters/validaters

DIR_BIN:=$(DEST_DIR)bin/
DIR_APP:=app/
DIR_SRC:=src/
DIR_HDR:=include/
DIR_SO:=$(DEST_DIR)lib/
DIR_O:=$(DEST_DIR)obj/
DIR_DEP:=$(DIR_O)

MERGE=$(subst $(SPACE),$1,$(sort $2))
SPLIT=$(subst $1,$(SPACE),$2)

vpath %.cpp $(DIR_APP)
vpath %.cpp $(DIR_SRC)
vpath %.hpp $(DIR_HPP)
vpath %.tpp $(DIR_HDR)
#vpath %.o $(DIR_O)
#vpath %.so $(DIR_SO)

DIRS_OBJ:=$(call MERGE,:,$(DIR_SO) $(DIR_O))
override RPATH:=-Wl,-rpath,$(call MERGE,:,$(call SPLIT,:,$(RPATH) $(DIRS_OBJ)))
override CXXFLAGS+=$(strip $(addprefix -I,$(sort $(DIR_HDR))))\
	$(sort $(call CONFIG_O,$(REQ_ALL)))
#override LDLIBS+=$(sort $(call CONFIG_SO,$(REQ_ALL)))
SO_LDLIBS:=$(LDLIBS)
EXE_LDLIBS:=$(LDLIBS)
override LDFLAGS+=$(RPATH) $(strip $(addprefix -L,$(sort $(DIR_SO) $(DIR_O)))) # TODO

PAT_APP=$(1:%=$(DIR_APP)%.cpp)
PAT_CPP=$(1:%=$(DIR_SRC)%.cpp)
PAT_HPP=$(1:%=$(DIR_HDR)%.hpp)
PAT_TPP=$(1:%=$(DIR_HDR)%.tpp)
PAT_D=$(1:%=$(DIR_DEP)%.d)
PAT_TD=$(1:%=$(DIR_DEP)%.Td)
PAT_O=$(1:%=$(DIR_O)%.o)
PAT_SO=$(1:%=$(DIR_SO)lib%.so)
PAT_EXE=$(1:%=$(DIR_BIN)%)

# Pattern inverses; both paths are taken so that the pipeline is flexible;
# sources and targets can be added/removed/etc. beyond their automatic
# discovery/generation.
UNPAT_APP=$(1:$(DIR_APP)%.cpp=%)
UNPAT_CPP=$(1:$(DIR_SRC)%.cpp=%)
UNPAT_HPP=$(1:$(DIR_HDR)%.hpp=%)
UNPAT_TPP=$(1:$(DIR_HDR)%.hpp=%)
UNPAT_D=$(1:$(DIR_DEP)%.d)
UNPAT_TD=$(1:$(DIR_DEP)%.Td)
UNPAT_EXE=$(1:$(DIR_BIN)%=%)
UNPAT_SO=$(1:$(DIR_SO)lib%.so=%)
UNPAT_O=$(1:$(DIR_O)%.o=%)

# Row-major: { for(x:$1) for(y:$2) x(y) }
DO_EA=$(foreach L,$1,$(call $L,$2))
# Composed row-major: { ... for(z:$3) x(y(z)) }
DO_EA2=$(call DO_EA,$1,$(call DO_EA,$2,$3))

# Col-major: { for(y:$2) for(x:$1) x(y) }
XDO_EA=$(foreach X,$2,$(call DO_EA,$1,$X))
# Composed col-major: { for(z:$3) ... x(y(z)) }
XDO_EA2=$(foreach X,$(call XDO_EA,$2,$3),$(call DO_EA,$1,$X))

# Maps one file path to another through patterns
REPAT=$(call PAT_$2,$(call UNPAT_$1,$3))
# Maps one file path to several
REPAT_EA=$(call DO_EA2,$(2:%=PAT_%),UNPAT_$1,$3)

# Convenience compositions with wildcard
WPAT=$(wildcard $(call PAT_$1,$2))
WREPAT=$(wildcard $(call REPAT,$1,$2,$3))
WDO_EA=$(wildcard $(call DO_EA,$1,$2))
WDO_EA2=$(wildcard $(call DO_EA2,$1,$2,$3))
WREPAT_EA=$(wildcard $(call REPAT_EA,$1,$2,$3))

# Splits one patterned file into a file pair
#   Note - this still allows identities like PAT_EXE/UNPAT_EXE
CROSS=$(foreach X,$4,$(call REPAT,$1,$2,$X):$(call REPAT,$1,$3,$X))
# Identical to 'pkg-config $1 $2' except for '$2'=='';
#   '--silence-errors' and redirection just swap false- / false+.
PKG_CONFIG=$(foreach N,$2,$(shell pkg-config $1 $N))
CONFIG_O=$(call PKG_CONFIG,--cflags,$1)
CONFIG_SO=$(call PKG_CONFIG,--libs,$1)

# Preserve alternate application sources from configuration
override SRCS_EXE+=$(call WPAT,APP,*)
override HDRS_EXE:=$(wildcard $(SRCS_EXE:%.cpp=%.hpp %.tpp))
# Only remove this if app and library byproducts are separated!
#   obj/.o,.d,.Td files will collide until the next Makefile version.
override SRCS_SO:=$(filter-out $(SRCS_EXE),$(SRCS_SO) $(call WPAT,CPP,*))

$(eval NAMES_EXE:=$(sort $(call UNPAT_APP,$(SRCS_EXE))))
$(eval NAMES_SO:=$(sort $(call UNPAT_CPP,$(SRCS_SO))))
$(eval FILES_EXE:=$(sort $(call REPAT,APP,EXE,$(SRCS_EXE))))
$(eval FILES_SO:=$(sort $(call REPAT,CPP,SO,$(SRCS_SO))))

TARGET?=$(firstword $(FILES_EXE))
default: $(TARGET) $(COMPLETE)

# Examples - uncomment and make 'vars-TESTS'
#E:=$(NAMES_EXE)
#EA:=$(FILES_EXE)
#EB:=$(SRCS_EXE)
#L:=$(NAMES_SO)
#LA:=$(FILES_SO)
#LB:=$(SRCS_SO)
#$(eval TEST1:=$(call DO_EA,PAT_HPP PAT_CPP,$L))
#$(eval TEST2:=$(call WDO_EA,PAT_HPP PAT_CPP PAT_APP,$L))
#$(eval TEST3:=$(call XDO_EA,PAT_HPP PAT_CPP,$L))
#$(eval TEST4:=$(call DO_EA2,dir notdir,PAT_APP PAT_HPP,$L))
#$(eval TEST5:=$(call XDO_EA2,dir notdir,PAT_APP PAT_HPP,$L))
#$(eval TEST6:=$(join $E,$(join $(EA:%=:%),$(EB))))
#$(eval TEST7:=$(call CROSS,SO,CPP,HPP,$(LA)))
#$(eval TEST8:=$(call REPAT_EA,SO,HPP TPP,$(LA)))
#$(eval TEST9:=$(call WREPAT_EA,SO,HPP TPP,$(LA)))
#$(eval TESTS:=$(foreach N,1 3 6 7 8 9,TEST$N))


NULL?=/dev/null
SOURCE=$(shell echo $(shell cat $(wildcard $(call PAT_D,$1)) $(NULL)))
EXTRACT=$(filter-out $(call PAT_HPP,$1) $(call PAT_CPP,$1),$(call SOURCE,$1))
CPP_EXTRACT=$(call WREPAT,HPP,CPP,$(call EXTRACT,$1))
D_EXTRACT=$(call REPAT,CPP,D,$(call CPP_EXTRACT,$1))
O_EXTRACT=$(call REPAT,CPP,O,$(call CPP_EXTRACT,$1))
SO_EXTRACT=$(call REPAT,CPP,SO,$(call CPP_EXTRACT,$1))
LD_EXTRACT=$(addprefix -l,$(call XDO_EA,UNPAT_CPP,$(call CPP_EXTRACT,$1)))

$(COMPLETE): $(FILES_SO) $(FILES_EXE); @echo $(CXXFLAGS) > $@
.PRECIOUS: %.d $(COMPLETE)

#temp.txt: override CXXFLAGS+=$(shell date)
#temp.txt:; @echo $(CXXFLAGS) >>$@

val-%:; @echo $($*)
var-%:; @echo '"$$($*)"="$($*)"'
vars-%:; @$(foreach V,$($*),echo '$V={$($V)}';)
.PHONY: val-% var-% vars-% clean-% clean

# Now clean-main removes bin/main, and clean-bin and clean-EXE are the same
#   - but this is too greedy to run without a --dry-run pass, which outweighs
#   or negates the benefit of convenience for the uninitiated.
clean-%:; @rm -i -f $(wildcard $(filter $*% %$* %$*%,\
	$(foreach V,EXE SO O D TD,$(call PAT_$V,$(NAMES_EXE) $(NAMES_SO))))\
	$(call DO_EA,$(*:%=PAT_%),$(NAMES_EXE) $(NAMES_SO)))
clean: $(addprefix clean-,EXE SO O D COMPLETE)

# show-%: override LDLIBS+=$(call LD_EXTRACT,$*)
show-%:
	@echo CPP_EXTRACT"($*)" = $(call CPP_EXTRACT,$*)
	@echo SO_EXTRACT"($*)" = $(call SO_EXTRACT,$*)
	@echo LD_EXTRACT"($*)" = $(call LD_EXTRACT,$*)
	@echo "  -->" $(call PAT_O,$*) : $(call SO_EXTRACT,$*)
	@echo "  --> LDLIBS = " $(LDLIBS)

$(foreach X,$(NAMES_EXE) $(NAMES_SO),$(eval $(call PAT_D,$X):))

# E...: E: O(E)
#$(call PAT_D,$(NAMES_EXE) $(NAMES_SO));

#$(foreach X,$(NAMES_EXE),$(eval $X: show-$X))

$(DIR_BIN)%: $(foreach N,CPP O SO,$(call $N_EXTRACT,$*))
$(DIR_SO)lib%.so: $(foreach N,CPP O SO,$(call $N_EXTRACT,$*))

$(call PAT_EXE,$(NAMES_EXE)): \
$(DIR_BIN)%: $(DIR_O)%.o $(DIR_O)%.d
	$(CXX) $(LDFLAGS) \
		-o $(shell echo $@ $< $(LDLIBS) $($*_LDLIBS) \
		$(sort $(call CONFIG_SO,$(REQ_$*))) $(call LD_EXTRACT,$(call UNPAT_EXE,$@)))
#	@echo "CPP_EXTRACT=$(call CPP_EXTRACT,$(call UNPAT_EXE,$@))"
#	@echo "O_EXTRACT=$(call O_EXTRACT,$(call UNPAT_EXE,$@))"
#	@echo "SO_EXTRACT=$(call SO_EXTRACT,$(call UNPAT_EXE,$@))"
#@echo '$$*=$*; $$@=$@; $$^=$^'
#@echo 'LD_EXTRACT=$(call LD_EXTRACT,$(call UNPAT_EXE,$@))'
# E...: O(E): APP(E)
$(foreach N,$(NAMES_EXE),$(eval $(call PAT_O,$N): $(call PAT_APP,$N) $(call PAT_D,$N); \
	$(CXX) $(CXXFLAGS)\
	$(sort $(call CONFIG_O,$(REQ_$N))) -c \
	-o $$@ $$<))
# SO...: SO: O(SO)
$(call PAT_SO,$(NAMES_SO)): \
$(DIR_SO)lib%.so: $(DIR_O)%.o
	$(CXX) $(shell echo $(LDFLAGS) $< -shared) \
		-o $(shell echo $@ $(LDLIBS) $($*_LDLIBS) $(sort $(call CONFIG_SO,$(REQ_$*)))\
		$(call LD_EXTRACT,$(call UNPAT_SO,$@)))
# O(SO...): O: CPP(O)
$(call PAT_O,$(NAMES_SO)): \
$(DIR_O)%.o: $(DIR_SRC)%.cpp $(DIR_DEP)%.d
	$(CXX) $(CXXFLAGS) $(sort $(call CONFIG_O,$(REQ_$*))) -fPIC -c \
		-o $@ $(call PAT_SRC,$*) $<

# Generate auto-dep injection - what could go wrong?
-include $(call PAT_D,$(NAMES_EXE) $(NAMES_SO))

include Doxygen.mk
