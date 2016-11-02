del shaders.h
for %%f in (*.frag) do (
rem			glslangValidator "%%~nf.frag"
            xxd -i "%%~nf.frag" >> shaders.h
)
for %%f in (*.vert) do (
rem			glslangValidator "%%~nf.vert"
            xxd -i "%%~nf.vert" >> shaders.h
)