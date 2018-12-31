appName = DiceWars
libName = libIA

srcExt = c
hExt = h

aiSrcDir = IA/src
aiObjDir = IA/obj
aiBinDir = DiceWars/IA

gameSrcDir = JEU/src
gameObjDir = JEU/obj
gameBinDir = DiceWars

debug = 1

aiCFlags = -fPIC
aiLDFlags =

CFlags = -Wall
LDFlags = -rdynamic

aiLibs =
aiLibDir =
aiIncDir =

gameLibs = png m SDL2_ttf SDL2_image SDL2 SDL2main dl
gameLibDir = SDL2/lib/
gameIncDir = SDL2/include/


###

ifeq ($(debug),1)
	debug=-ggdb
else
	debug=
endif

gameIncDir := $(addprefix -I, $(gameIncDir))
gameLibs := $(addprefix -l, $(gameLibs))
gameLibDir := $(addprefix -L, $(gameLibDir))

aiCFlags += -c $(debug) $(aiIncDir)
aiLDFlags += $(aiLibDir) $(aiLibs)

CFlags += -c $(debug) $(gameIncDir)
LDFlags += $(gameLibDir) $(gameLibs)

gameSources := $(shell find $(gameSrcDir) -name '*.$(srcExt)')
#gameSrcDir := $(shell find . -name '*.$(srcExt)' -exec dirname {} \; | sort | uniq)
gameObjects := $(patsubst %.$(srcExt),$(gameObjDir)/%.o,$(gameSources))

aiSources := $(shell find $(aiSrcDir) -name '*.$(srcExt)')
#aiSrcDir := $(shell find . -name '*.$(srcExt)' -exec dirname {} \; | sort | uniq)
aiObjects := $(patsubst %.$(srcExt),$(aiObjDir)/%.o,$(aiSources))

ifeq ($(srcExt),cpp)
	CC = $(CXX)
else
	CFlags += -std=gnu99
	aiCFlags += -std=gnu99
endif

.PHONY: all $(appName) $(libName)

all: distclean $(appName) $(libName)

$(appName): distclean $(gameObjects)
	@mkdir -p `dirname $@`
	@echo "Linking $@..."
	@$(CC) $(gameObjects) $(LDFlags) -o $(gameBinDir)/$@

$(gameObjDir)/%.o: %.$(srcExt) %.$(hExt)
	@echo "Generating dependencies for $<..."
	@mkdir -p `dirname $(gameObjDir)/$<`
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@echo "Compiling $<..."
	@$(CC) $(CFlags) $< -o $@

$(libName): distclean $(aiObjects)
	@mkdir -p `dirname $@`
	@echo "Linking $@..."
	@$(CC) -shared -fPIC $(aiObjects) $(aiLDFlags) -o $(aiBinDir)/$@.so

$(aiObjDir)/%.o: %.$(srcExt) %.$(hExt)
	@echo "Generating dependencies for $<..."
	@mkdir -p `dirname $(aiObjDir)/$<`
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@echo "Compiling $<..."
	@$(CC) $(aiCFlags) $< -o $@

clean:
	@echo "Deleting object and dependencies files"
	@$(RM) -r $(gameObjDir)
	@$(RM) -r $(aiObjDir)

distclean:
	@echo "Deleting bin, object and dependencies files"
	@$(RM) -r $(gameObjDir)
	@$(RM) -r $(aiObjDir)
	@$(RM) $(gameBinDir)/$(appName)
	@$(RM) $(aiBinDir)/$(libName).so
