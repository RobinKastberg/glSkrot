del shaders.h
for %%f in (*.frag) do (
            echo %%~nf
            xxd -i "%%~nf.frag" >> shaders.h
)
for %%f in (*.vert) do (
            echo %%~nf
            xxd -i "%%~nf.vert" >> shaders.h
)
pause