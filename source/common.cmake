# ��ӡ��־(����CMake������·�������ڲ鿴)
function(print_string str)
    message(STATUS "${str}")
endfunction()

###############################################################################
# ����Ŀ¼�µ�hlsl�ļ�����������ĺ����������ǵ����ԡ�
# Ŀ���Ǳ�������hlsl��Ҫ����������ֶ����ã�
#   set_hlsl_file_prop(default_vs Vertex)
#   set_hlsl_file_prop(default_ps Pixel)
###############################################################################
function(batch_set_hlsl_file_props dir)
    print_string("Search in '${dir}' ...")
    # �����ļ�
    file(GLOB hlsl_files "${dir}/*.hlsl")
    foreach(fpath ${hlsl_files})
        string(FIND ${fpath} "/" last_slash_index REVERSE)      # ��������һ��'/'
        math(EXPR last_slash_index "${last_slash_index} + 1")   # last_slash_index++
        string(SUBSTRING ${fpath} ${last_slash_index} -1 fname) # ��ȡ'/'����ļ���
        string(REPLACE ".hlsl" "" fname_no_ext ${fname})        # ȥ���ļ���չ��
        string(FIND ${fname_no_ext} "_vs" is_vs)                # ����`_vs`
        string(FIND ${fname_no_ext} "_ps" is_ps)                # ����`_ps`
        if(NOT ${is_vs} EQUAL -1)
            set_hlsl_file_prop(${fname_no_ext} Vertex)
            print_string("Find vertex shader: ${fname_no_ext}")
        elseif(NOT ${is_ps} EQUAL -1)
            set_hlsl_file_prop(${fname_no_ext} Pixel)
            print_string("Find pixel shader: ${fname_no_ext}")
        endif()
    endforeach()
endfunction()
