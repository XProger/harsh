compile: check

	@echo
	@echo "--------------------- Harsh Engine ---------------------"
	@echo

	$(AS3COMPILERARGS) -AS3 -strict \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/CModule.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		Console.as -outdir . -out Console

	$(AS3COMPILERARGS) -AS3 -strict \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/CModule.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		-import Console.abc \
		PreLoader.as -swf com.adobe.flascc.preloader.PreLoader,800,600,60 -outdir . -out PreLoader
	
	"$(FLASCC)/usr/bin/g++" main.cpp \
	../src/core.cpp \
	../src/utils.cpp \
	../src/renderS3D.cpp \
	../src/resource.cpp \
	../src/game.cpp \
	../src/scene.cpp \
	-I../src/ \
	-DFLASH \
	-swf-version=14 \
	-swf-preloader=PreLoader.swf \
	-swf-size=800x600 \
	-lAS3++ -lFlash++ \
	-jvmopt="-Xmx1G" \
	-symbol-abc=Console.abc \
	-emit-swf -O3 -o harsh.swf

clean:
	rm *.swf *.abc

include Makefile.common
