#___________________________________________________________________[MAKEFILE]
#/////////////////////////////////////////////////////////////////////////////
# @(#)%P%
# @(#)Purpose:     Install icons
# Targets:         all      -- install
# @(#)Author:      R. Lutter
# @(#)Revision:    SCCS: %W%
# @(#)Date:        %G%
# URL:             
#/////////////////////////////////////////////////////////////////////////////

all:		install

install:	CheckMrbPath

ICONS 		=	\
				arrow_leftleft.xpm \
				arrow_rightright.xpm \
				cbutton_all.xpm \
				cbutton_none.xpm \
				exit.xpm \
				openbook.xpm \
				open.xpm \
				save.xpm \
				save_all.xpm

DEST 		=	$(MARABOU)/icons

INSTALL 	= :
NOINSTALL	=	

.SUFFIXES:	.xpm

all:		install

install:	CheckMrbPath $(ICONS)
			@$(INSTALL) for i in $(M_SERVER); \
			$(INSTALL)	do \
			$(INSTALL)  	echo "[`pwd`: Installing icons on $$i:$(DEST)]"; \
			$(INSTALL)		scp $(ICONS) $$i:$(DEST); \
			$(INSTALL)	done

			@$(NOINSTALL) echo "all: Nothing to do."

nd:
			@nd -a $(ICONS)

nv:
			@nv $(ICONS)

ad-n:
			@for i in $(ICONS); \
			do \
				if [ ! -f s.$$i ]; \
				then \
					echo "ad-n $$i"; \
					rm -f p.$$i; \
					ad-n $$i; \
					rm $$i; \
					nv $$i; \
				fi; \
			done

CheckMrbPath:
			@if [ -z "$(ROOTSYS)" ]; then echo "$(MAKE): path to ROOT system (ROOTSYS) undefined"; fi; \
			if [ -z "$(MARABOU)" ]; then echo "$(MAKE): path to MARaBOU system (MARABOU) undefined"; fi; \
			if [ -z "$(M_SERVER)" ]; then echo "$(MAKE): name(s) of server(s) (M_SERVER) to install files undefined"; fi; \
			if [ -z "$(ROOTSYS)" -o -z "$(MARABOU)" -o -z "$(M_SERVER)" ]; then exit 1; fi;
			@echo "[$(MAKE) @ `pwd`: MARABOU=$(MARABOU)]";
