#
GSLLIBS=-lgsl -lgslcblas
#
HARDYHERON64=-fPIC

ARCH = $(shell uname)
LEX=flex
YACC=bison -d -v -t
LEXLIB = -lfl
DLLLIB = -ldl
ifeq ($(ARCH),Darwin)
  DLLLIB =
endif
ifeq ($(ARCH),OpenBSD)
  DLLLIB =
endif

EXRINCLUDE=-I/usr/include/OpenEXR
EXRLIBDIR=-L/usr/lib/x86_64-linux-gnu


EXRLIBS=$(EXRLIBDIR) -Bstatic -lIex -lIlmImf -lImath -lIex -lHalf -Bdynamic -lz
ifeq ($(ARCH),Linux)
  EXRLIBS += -lpthread
endif

CC=mpicc
CXX=mpicxx

LD=$(CXX) $(OPT)
OPT=-O2 -g -O0
# OPT=-O2 -msse -mfpmath=sse
INCLUDE=-I. -Icore -Imcm -ImcmCodage $(EXRINCLUDE)
WARN=-Wall
CWD=$(shell pwd)
#CXXFLAGS=$(OPT) $(INCLUDE) $(WARN)
CXXFLAGS=$(OPT) $(INCLUDE) $(WARN) $(HARDYHERON64)
CCFLAGS=$(CXXFLAGS)
#LIBS=$(LEXLIB) $(DLLLIB) $(EXRLIBDIR) $(EXRLIBS) -lm 
LIBS=$(LEXLIB) $(DLLLIB) $(EXRLIBDIR) $(EXRLIBS) $(GSLLIBS) -lm 

SHARED_LDFLAGS = -shared
LRT_LDFLAGS=-rdynamic $(OPT)
#PBRTPRELINK=-Wl,--export-dynamic -Wl,-whole-archive
#PBRTPOSTLINK=-Wl,-no-whole-archive

ifeq ($(ARCH), Darwin)
  OS_VERSION = $(shell uname -r)
  SHARED_LDFLAGS = -flat_namespace -undefined suppress -bundle #-noprebind
  LRT_LDFLAGS=$(OPT) # -L/sw/lib
  INCLUDE += -I/sw/include
#  WARN += -Wno-long-double
endif

ACCELERATORS = grid kdtree
CAMERAS      = environment orthographic perspective
CORE         = api camera color dynload exrio film geometry light material mc \
               paramset parser primitive reflection sampling scene shape \
               texture timer transform transport util volume pbrtparse pbrtlex
CORE_SCENE   = scene
FILM         = image
FILTERS      = box gaussian mitchell sinc triangle
INTEGRATORS  = directlighting emission irradiancecache \
               path photonmap single whitted igi debug exphotonmap
LIGHTS       = area distant goniometric infinite point projection spot infinitesample
MATERIALS    = bluepaint brushedmetal clay felt \
               glass matte mirror plastic primer \
               shinymetal skin substrate translucent uber mcmMatte mcmMirror
SAMPLERS     = bestcandidate lowdiscrepancy random stratified
SHAPES       = cone cylinder disk heightfield hyperboloid loopsubdiv nurbs \
               paraboloid sphere trianglemesh wavefront \
               heliostat truncatedcone H1 H1F1
TEXTURES     = bilerp checkerboard constant dots fbm imagemap marble mix \
               scale uv windy wrinkled
TONEMAPS     = contrast highcontrast maxwhite nonlinear
VOLUMES      = exponential homogeneous volumegrid

RENDERER     = pbrt
MCM          = mcm



RENDERER_OBJS     := $(addprefix objs/, $(RENDERER:=.o) )
CORE_OBJS         := $(addprefix objs/, $(CORE:=.o) )
CORE_LIB          := core/libpbrt.a
MCM_OBJS          := $(addprefix objs/, $(MCM:=.o) )

SHAPES_DSOS       := $(addprefix bin/, $(SHAPES:=.so))
MATERIALS_DSOS    := $(addprefix bin/, $(MATERIALS:=.so))
LIGHTS_DSOS       := $(addprefix bin/, $(LIGHTS:=.so))
INTEGRATORS_DSOS  := $(addprefix bin/, $(INTEGRATORS:=.so))
VOLUMES_DSOS      := $(addprefix bin/, $(VOLUMES:=.so))
TEXTURES_DSOS     := $(addprefix bin/, $(TEXTURES:=.so))
ACCELERATORS_DSOS := $(addprefix bin/, $(ACCELERATORS:=.so))
CAMERAS_DSOS      := $(addprefix bin/, $(CAMERAS:=.so))
FILTERS_DSOS      := $(addprefix bin/, $(FILTERS:=.so))
FILM_DSOS         := $(addprefix bin/, $(FILM:=.so))
TONEMAPS_DSOS     := $(addprefix bin/, $(TONEMAPS:=.so))
SAMPLERS_DSOS     := $(addprefix bin/, $(SAMPLERS:=.so))

SHAPES_OBJS       := $(addprefix objs/, $(SHAPES:=.o))
MATERIALS_OBJS    := $(addprefix objs/, $(MATERIALS:=.o))
LIGHTS_OBJS       := $(addprefix objs/, $(LIGHTS:=.o))
INTEGRATORS_OBJS  := $(addprefix objs/, $(INTEGRATORS:=.o))
VOLUMES_OBJS      := $(addprefix objs/, $(VOLUMES:=.o))
TEXTURES_OBJS     := $(addprefix objs/, $(TEXTURES:=.o))
ACCELERATORS_OBJS := $(addprefix objs/, $(ACCELERATORS:=.o))
CAMERAS_OBJS      := $(addprefix objs/, $(CAMERAS:=.o))
FILTERS_OBJS      := $(addprefix objs/, $(FILTERS:=.o))
FILM_OBJS         := $(addprefix objs/, $(FILM:=.o))
TONEMAPS_OBJS     := $(addprefix objs/, $(TONEMAPS:=.o))
SAMPLERS_OBJS     := $(addprefix objs/, $(SAMPLERS:=.o))

RENDERER_BINARY = bin/mcm3d

CORE_HEADERFILES = api.h camera.h color.h dynload.h film.h geometry.h \
                  kdtree.h light.h pbrt.h material.h mc.h mipmap.h octree.h \
                  paramset.h primitive.h reflection.h sampling.h scene.h \
                  shape.h texture.h timer.h tonemap.h transform.h transport.h \
                  volume.h

CORE_HEADERS := $(addprefix core/, $(CORE_HEADERFILES) )

MCM_HEADERFILES = mcm.h

MCM_HEADERS := $(addprefix mcm/, $(MCM_HEADERFILES) )

MCMCODAGE_HEADERFILES = mcmAlgo.h mcmEntrees.h mcmMethodes.h mcmSorties.h mcmVariables.h

MCMCODAGE_HEADERS := $(addprefix mcmCodage/, $(MCMCODAGE_HEADERFILES) )

.SECONDARY: $(SHAPES_OBJS) $(MATERIALS_OBJS) $(LIGHTS_OBJS) $(INTEGRATORS_OBJS) \
            $(VOLUMES_OBJS) $(ACCELERATORS_OBJS) $(CAMERAS_OBJS) $(FILTERS_OBJS) \
            $(FILM_OBJS) $(TONEMAPS_OBJS) $(SAMPLERS_OBJS) $(TEXTURES_OBJS)

.PHONY: tools exrcheck

default: $(CORE_LIB) $(RENDERER_BINARY) $(INTEGRATORS_DSOS) $(VOLUMES_DSOS) $(FILM_DSOS) $(SHAPES_DSOS) $(MATERIALS_DSOS) $(LIGHTS_DSOS) $(ACCELERATORS_DSOS) $(CAMERAS_DSOS) $(SAMPLERS_DSOS) $(FILTERS_DSOS) $(TONEMAPS_DSOS) $(TEXTURES_DSOS) #tools

tools: $(CORE_LIB)
	(cd tools && $(MAKE))

$(CORE_LIB): $(CORE_OBJS)
	@echo "Building the core rendering library (libpbrt.a)"
	@ar rcs $(CORE_LIB) $(CORE_OBJS)

bin/%.so: objs/%.o 
	@$(LD) $(SHARED_LDFLAGS) $^ -o $@

objs/%.o: renderer/%.cpp $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Building the rendering binary (pbrt)"
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/%.o: mcm/%.cpp $(MCM_HEADERS)
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/%.o: core/%.cpp $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/scene.o: core/scene.cpp $(CORE_HEADERS) $(MCM_HEADERS) $(MCMCODAGE_HEADERS)
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/%.o: core/%.c $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Compiling $<"
	@$(CC) $(CCFLAGS) -o $@ -c $<

objs/%.o: shapes/%.cpp $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Building Shape Plugin \"$*\""
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/%.o: integrators/%.cpp $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Building Integrator Plugin \"$*\""
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/%.o: volumes/%.cpp $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Building Volume Plugin \"$*\""
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/%.o: textures/%.cpp $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Building Texture Plugin \"$*\""
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/%.o: materials/%.cpp $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Building Material Plugin \"$*\""
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/%.o: lights/%.cpp $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Building Light Plugin \"$*\""
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/%.o: accelerators/%.cpp $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Building Accelerator Plugin \"$*\""
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/%.o: cameras/%.cpp $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Building Camera Plugin \"$*\""
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/%.o: filters/%.cpp $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Building Filter Plugin \"$*\""
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/%.o: tonemaps/%.cpp $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Building Tone Map Plugin \"$*\""
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/%.o: film/%.cpp $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Building Film Plugin \"$*\""
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

objs/%.o: samplers/%.cpp $(CORE_HEADERS) $(MCM_HEADERS)
	@echo "Building Sampler Plugin \"$*\""
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

core/pbrtlex.cpp: core/pbrtlex.l
	@echo "Lex'ing pbrtlex.l"
	@$(LEX) -o$@ core/pbrtlex.l

core/pbrtparse.h core/pbrtparse.cpp: core/pbrtparse.y
	@echo "YACC'ing pbrtparse.y"
	@$(YACC) -o $@ core/pbrtparse.y
	@if [ -e core/pbrtparse.cpp.h ]; then /bin/mv core/pbrtparse.cpp.h core/pbrtparse.h; fi
	@if [ -e core/pbrtparse.hpp ]; then /bin/mv core/pbrtparse.hpp core/pbrtparse.h; fi

#$(RENDERER_BINARY): $(RENDERER_OBJS) $(CORE_LIB)
$(RENDERER_BINARY): $(RENDERER_OBJS) $(MCM_OBJS) $(CORE_LIB)
	@echo "Linking $@"
#	@$(CXX) $(LRT_LDFLAGS) -o $@ $(RENDERER_OBJS) $(PBRTPRELINK) $(CORE_OBJS) $(PBRTPOSTLINK) $(LIBS)
	@$(CXX) $(LRT_LDFLAGS) -o $@ $(RENDERER_OBJS) $(MCM_OBJS) $(PBRTPRELINK) $(CORE_OBJS) $(PBRTPOSTLINK) $(LIBS)

clean:
	rm -f */*.o */*.so */*.a bin/mcm3d core/pbrtlex.[ch]* core/pbrtparse.[ch]*
	(cd tools && $(MAKE) clean)

clean-algo:
	rm -f */scene.o bin/mcm3d

objs/exrio.o: exrcheck

exrcheck:
	@echo -n Checking for EXR installation... 
	@$(CXX) $(CXXFLAGS) -o exrcheck exrcheck.cpp $(LIBS) || \
		(cat exrinstall.txt; exit 1)
