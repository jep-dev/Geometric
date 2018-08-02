# Tools
doxygen?=doxygen
doxymake=$(doxygen) -g

# Input parameters
DIR_DOC?=doc/
DIR_TAG?=$(DIR_DOC)tags/
FILE_DOC?=$(DIR_DOC)Doxyfile

HTML_EXTRA_STYLESHEET?=doc/style.css
BUILTIN_STL_SUPPORT?=YES
COLS_IN_ALPHA_INDEX?=5
DOT_TRANSPARENT?=YES
EXCLUDE_PATTERNS?=*.o, *.d, *.h, */obj/*
EXTRACT_ALL?=YES
EXTRACT_PRIVATE?=YES
EXTRACT_PACKAGE?=YES
INPUT=$(DIR_HDR) $(DIR_SRC) $(DIR_APP)
INPUTS=$(foreach I,$(INPUT),$(wildcard $I*))
OUTPUT_DIRECTORY=$(DIR_DOC)
QT_AUTOBRIEF?=YES
MULTILINE_CPP_IS_BRIEF=YES
RECURSIVE?=YES
SHOW_INCLUDE_FILES?=YES
STRIP_FROM_PATH?=./
WARNINGS=NO

TAG_DIRNAMES?=html latex rtf man
TAG_FILENAMES:=$(TAG_DIRNAMES:%=$(DIR_DOC)%.tag)
TAG_FILEMAPS:=$(join $(TAG_FILENAMES),$(addprefix =,$(TAG_FILENAMES:.tag=)))

DOXY_LOCATE=$(foreach D,HDR SRC APP,$(foreach X,.hpp .tpp .cpp,$(wildcard $(DIR_$D)*$X)))

doxy-reset:
	@$(doxymake) $(FILE_DOC) >$(NULL)
	$(eval DOXY_VARS:=HTML_EXTRA_STYLESHEET BUILTIN_STL_SUPPORT COLS_IN_ALPHA_INDEX \
		DOT_TRANSPARENT EXCLUDE_PATTERNS EXTRACT_ALL EXTRACT_PRIVATE EXTRACT_PACKAGE INPUT \
		OUTPUT_DIRECTORY QT_AUTOBRIEF MULTILINE_CPP_IS_BRIEF RECURSIVE SHOW_INCLUDE_FILES \
		STRIP_FROM_PATH WARNINGS)

$(DIR_DOC)%.tag: doxy-reset $(DOXY_LOCATE)
	echo $(foreach VAR,$(DOXY_VARS),"$(VAR)=$($(VAR))\n") \
		GENERATE_TAGFILE="$@\n" \
		GENERATE_$(call TO_UPPER,$*)="YES\n" \
		$(addprefix GENERATE_,$(foreach N,$(TAG_DIRNAMES),\
		$(addsuffix ="NO\n",$(call TO_UPPER,$(filter-out $*,$N))))) \
		>>$(FILE_DOC) && $(doxygen) $(FILE_DOC) >$(NULL)

$(TAG_DIRNAMES): %: $(DIR_DOC)%.tag
doc: $(TAG_FILENAMES)

clean-doc:; $(RM) $(FILE_DOC) $(TAG_FILENAMES)
.PHONY: doc doxy-reset clean-doc $(TAG_DIRNAMES)
