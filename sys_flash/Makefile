compile: check

	@echo
	@echo "--------------------- Harsh Engine ---------------------"
	@echo

	$(AS3COMPILERARGS) -AS3 -strict \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		AGALAssembler/*.as -outdir . -out AGAL	

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

	java -jar $(call nativepath, $(FLASCC)/usr/lib/as3wig.jar) -i AGAL.abc -o AGAL
	"$(FLASCC)/usr/bin/g++" AGAL.cpp -c -emit-llvm &> AGAL.txt
	
	"$(FLASCC)/usr/bin/g++" main.cpp \
	../src/core.cpp \
	../src/utils.cpp \
	../src/renderS3D.cpp \
	../src/resource.cpp \
	../src/game.cpp \
	../src/scene.cpp \
	-swf-version=15 \
	-swf-preloader=PreLoader.swf \
	-fllvm-llc-opt=-ascopt=-import \
	-fllvm-llc-opt=-ascopt=$(call nativepath,AGAL.abc) \
	AGAL.o AGAL.abc -lAS3++ -lFlash++ \
	-DFLASH \
	-I../src/ \
	-jvmopt="-Xmx1G" \
	-swf-size=800x600 \
	-symbol-abc=Console.abc \
	-emit-swf -O0 -o harsh.swf

clean:
	rm -f stage3d.swf rm AGAL.* AGAL_* *.abc *.o SpaceTriangle/*.abc

include Makefile.common
