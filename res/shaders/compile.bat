for /r %%f in (*) do (
    set "TRUE="
    If "%%~xf"==".vert" set TRUE=1
    If "%%~xf"==".frag" set TRUE=1
    If defined TRUE (
        "C:\Program Files\Vulkan\1.2.176.1\Bin32\glslc.exe" "%%~nf%%~xf" -o  "%%~nf%%~xf.spv"
    )
)
