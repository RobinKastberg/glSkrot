del shaders.h
for %%f in (*.frag) do (
			glslangValidator "%%~nf.frag"
            xxd -i "%%~nf.frag" >> shaders.h
)
for %%f in (*.vert) do (
			glslangValidator "%%~nf.vert"
            xxd -i "%%~nf.vert" >> shaders.h
)