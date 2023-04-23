# 打印日志(可在CMake界面的下方输出窗口查看)
function(print_string str)
    message(STATUS "${str}")
endfunction()

###############################################################################
# 遍历目录下的hlsl文件，调用上面的函数设置他们的属性。
# 目的是避免新增hlsl需要类似下面的手动调用：
#   set_hlsl_file_prop(default_vs Vertex)
#   set_hlsl_file_prop(default_ps Pixel)
###############################################################################
function(batch_set_hlsl_file_props dir)
    print_string("Search in '${dir}' ...")
    # 遍历文件
    file(GLOB hlsl_files "${dir}/*.hlsl")
    foreach(fpath ${hlsl_files})
        string(FIND ${fpath} "/" last_slash_index REVERSE)      # 查找最右一个'/'
        math(EXPR last_slash_index "${last_slash_index} + 1")   # last_slash_index++
        string(SUBSTRING ${fpath} ${last_slash_index} -1 fname) # 截取'/'后的文件名
        string(REPLACE ".hlsl" "" fname_no_ext ${fname})        # 去除文件扩展名
        string(FIND ${fname_no_ext} "_vs" is_vs)                # 查找`_vs`
        string(FIND ${fname_no_ext} "_ps" is_ps)                # 查找`_ps`
        if(NOT ${is_vs} EQUAL -1)
            set_hlsl_file_prop(${fname_no_ext} Vertex)
            print_string("Find vertex shader: ${fname_no_ext}")
        elseif(NOT ${is_ps} EQUAL -1)
            set_hlsl_file_prop(${fname_no_ext} Pixel)
            print_string("Find pixel shader: ${fname_no_ext}")
        endif()
    endforeach()
endfunction()
