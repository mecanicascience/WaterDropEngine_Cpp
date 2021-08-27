cd "../shaders/"
for /r %%f in (*) do (
    set "TRUE="
    If "%%~xf"==".vert" set TRUE=1
    If "%%~xf"==".frag" set TRUE=1
    If "%%~xf"==".comp" set TRUE=1
    If defined TRUE (
        "%VULKAN_SDK%\Bin32\glslc.exe" "%%f" -o  "%%f.spv"
    )
)
cd "../manager/"
