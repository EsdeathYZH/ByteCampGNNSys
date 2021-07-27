
function(bytegraph_test test_file)
    get_filename_component(test_target_name "${test_file}" NAME_WE)

    add_executable("${test_target_name}" "")
    target_sources("${test_target_name}"
            PRIVATE
            "${test_file}"
            )
    target_link_libraries("${test_target_name}" gmock gtest)
    target_compile_options("${test_target_name}" PUBLIC -g)
    add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
endfunction(bytegraph_test)

bytegraph_test("sample_test.cc")