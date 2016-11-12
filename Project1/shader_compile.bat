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
for %%f in (*.tesc) do (
rem			glslangValidator "%%~nf.tesc"
			echo extern unsigned char %%~nf_tesc[]; >> shaders_include.h
			echo extern unsigned int %%~nf_tesc_len; >> shaders_include.h
            xxd -i "%%~nf.tesc" >> shaders.h
)
for %%f in (*.tese) do (
rem			glslangValidator "%%~nf.tese"
			echo extern unsigned char %%~nf_tese[]; >> shaders_include.h
			echo extern unsigned int %%~nf_tese_len; >> shaders_include.h
            xxd -i "%%~nf.tese" >> shaders.h
)