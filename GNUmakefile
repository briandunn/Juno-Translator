#
# Juno Translator
#
# 2009 Brian Dunn brianpatrickdunn@gmail.com
#
#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

VERSION=0.1
NAME=juno-translator
LIBS = -lasound
INCLUDES = -I.

CC = gcc
CXX = g++

CFLAGS = -g -O0 $(INCLUDES) -Wall
CXXFLAGS = $(CFLAGS)
LDFLAGS = $(LIBS)

.PHONY: all checkgcc clean spotless realclean distclean mrproper diff gprof.txt depclean dist

SRC := $(wildcard *.cpp)

OBJ := $(addprefix gen/obj/, $(notdir $(SRC:.cpp=.o)))

all: gen/$(NAME)

ifeq ($(MAKECMDGOALS),)
  MYMAKECMDGOALS = x
else
  MYMAKECMDGOALS = $(MAKECMDGOALS)
endif

ifneq ($(filter-out clean spotless realclean distclean mrproper checkgcc issues diff tags,$(MYMAKECMDGOALS)),)
  -include $(addprefix gen/dep/, $(notdir $(OBJ:.o=.d)))
endif

gen/dep/%.d: %.cpp
	@mkdir -p gen/dep/
	@echo "<dep| $<"
	@$(CC) -M -MG $(CFLAGS) $< | sed -e 's@ /[^ ]*@@g' -e 's@^\(.*\)\.o:@gen/dep/\1.d gen/obj/\1.o:@' > $@

gen/obj/%.o: %.cpp
	@mkdir -p gen/obj/
	@echo "|cmp> $<"
	@$(CXX) -c $(CXXFLAGS) $< -o $@

gen/$(NAME): $(OBJ)
	@echo "=lnk= $@"
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LDFLAGS)

clean:
	-rm -rf gen
	-rm -f *~ make.log parser

dist:
	@$(MAKE) clean
	@rm -f ../$(NAME).tar.gz
	@rm -f $(NAME)-$(VERSION).tar.gz
	@(cd .. && tar czf $(NAME).tar.gz $(NAME))
	@tar xzf ../$(NAME).tar.gz
	@(cd $(NAME) && find . -name CVS | xargs rm -rf)
	@mv $(NAME) $(NAME)-$(VERSION)
	@tar czf $(NAME)-$(VERSION).tar.gz $(NAME)-$(VERSION)
	@ls -l $(NAME)-$(VERSION).tar.gz
	@rm -rf $(NAME)-$(VERSION)

depclean:
	rm -rf gen/dep
