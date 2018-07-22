# Tools
doxygen?=doxygen
doxymake=$(doxygen) -g

# Input parameters
DIR_DOC?=doc/
FILE_DOC?=$(DIR_DOC)Doxyfile

DOXY_HTML_EXTRA_STYLESHEET=doc/style.css
DOXY_BUILTIN_STL_SUPPORT?=YES
DOXY_COLS_IN_ALPHA_INDEX?=5
DOXY_DOT_TRANSPARENT?=YES
DOXY_EXCLUDE_PATTERNS?=*.o, *.d, *.h, */obj/*
DOXY_EXTRACT_ALL?=YES
DOXY_EXTRACT_PRIVATE?=YES
DOXY_EXTRACT_PACKAGE?=YES
#DOXY_GENERATE_MAN?=YES
DOXY_OUTPUT_DIRECTORY=$(DIR_DOC)
DOXY_QT_AUTOBRIEF?=YES
DOXY_MULTILINE_CPP_IS_BRIEF=YES
DOXY_RECURSIVE?=YES
DOXY_SHOW_INCLUDE_FILES?=YES
DOXY_STRIP_FROM_PATH?=./

DOXY_WARNINGS=NO

override DOXY_INPUT+=$(DOXY_USE_MDFILE_AS_MAINPAGE) $(SRCS_EXE) $(SRCS_SO) \
	$(call WPAT,HPP,*) $(call WPAT,TPP,*)

V_DOXY_ALL=$(filter DOXY_%,$(.VARIABLES))
V_DOXY_ALL_SUFFIXES=$(V_DOXY_ALL:DOXY_%=%)
V_DOXY_REPLACEMENT=$(foreach V,$(V_DOXY_ALL_SUFFIXES),\\n$(V) = $(DOXY_$(V)))

$(FILE_DOC): Doxygen.mk $(DOXY_INPUT)
	@$(doxymake) $(FILE_DOC) >/dev/null && \
		echo $(strip $(V_DOXY_REPLACEMENT)) >> $(FILE_DOC)

doc: Doxygen.mk $(FILE_DOC)
	@$(doxygen) $(FILE_DOC) >/dev/null
clean-doc:; $(RM) $(FILE_DOC)
.PHONY: doc clean-doc
