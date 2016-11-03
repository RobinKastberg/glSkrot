del shaders.h
del shaders_include.h
for %%f in (*.frag) do (
rem			glslangValidator "%%~nf.frag"
			echo extern unsigned char %%~nf_frag[]; >> shaders_include.h
			echo extern unsigned int %%~nf_frag_len; >> shaders_include.h
            xxd -i "%%~nf.frag" >> shaders.h
)
for %%f in (*.vert) do (
rem			glslangValidator "%%~nf.vert"
			echo extern unsigned char %%~nf_vert[]; >> shaders_include.h
			echo extern unsigned int %%~nf_vert_len; >> shaders_include.h
            xxd -i "%%~nf.vert" >> shaders.h
)