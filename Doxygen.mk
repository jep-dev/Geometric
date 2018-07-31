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
NAMES_DOC=$(patsubst %,doxy-tag-%,$(sort $(notdir $(basename $(INPUTS)))))
OUTPUT_DIRECTORY=$(DIR_DOC)
QT_AUTOBRIEF?=YES
MULTILINE_CPP_IS_BRIEF=YES
RECURSIVE?=YES
SHOW_INCLUDE_FILES?=YES
STRIP_FROM_PATH?=./
WARNINGS=NO

DOXY_LOCATE=$(wildcard $(DIR_HDR)$1.hpp $(DIR_HDR)$1.tpp $(DIR_SRC)$1.cpp $(DIR_APP)$1.cpp)

doxy-reset:
	@$(doxymake) $(FILE_DOC) >$(NULL)
	$(eval DOXY_VARS:=HTML_EXTRA_STYLESHEET BUILTIN_STL_SUPPORT COLS_IN_ALPHA_INDEX \
		DOT_TRANSPARENT EXCLUDE_PATTERNS EXTRACT_ALL EXTRACT_PRIVATE EXTRACT_PACKAGE INPUT \
		OUTPUT_DIRECTORY QT_AUTOBRIEF MULTILINE_CPP_IS_BRIEF RECURSIVE SHOW_INCLUDE_FILES \
		STRIP_FROM_PATH WARNINGS \
		$(foreach G,HTML LATEX RTF MAN,$(eval GENERATE_$G=NO)GENERATE_$G))

doxy-tag-% $(DIR_TAG)%.tag: doxy-reset $(call DOXY_LOCATE,$*)
	@$(doxymake) $(FILE_DOC) >$(NULL)
	@echo $(foreach VAR,$(DOXY_VARS),"$(VAR)=$($(VAR))\n") "GENERATE_TAGFILE=$(DIR_TAG)$*.tag" \
		>>$(FILE_DOC)
	@$(doxygen) $(FILE_DOC) >$(NULL)

$(FILE_DOC): Doxygen.mk $(wildcard $(DOXY_DISCOVER))
	@$(doxymake) $(FILE_DOC) >$(NULL)

doxy-tags: doxy-reset $(NAMES_DOC)

#doc: doxy-tags doxy-reset
#	@echo $(foreach VAR,$(DOXY_VARS),"$(VAR)=$($(VAR))\n") \
#		"GENERATE_HTML=YES\n" "GENERATE_LATEX=YES\n" "GENERATE_TAGFILE=\n" \
#		"TAGFILES=$(patsubst %,%=$(DIR_DOC)html/,$(wildcard $(DIR_TAG)*))" >>$(FILE_DOC)
#	@$(doxygen) $(FILE_DOC) >$(NULL)
doc: doxy-reset
	@echo $(foreach VAR,$(DOXY_VARS),"$(VAR)=$($(VAR))\n") \
		"GENERATE_HTML=YES\n" >>$(FILE_DOC)
	@$(doxygen) $(FILE_DOC) >$(NULL)

clean-doc:; $(RM) $(FILE_DOC)
.PHONY: doc doxy-reset doxy-tag doxy-tags doxy-tag-% clean-doc
