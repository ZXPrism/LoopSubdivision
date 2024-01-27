set_project("LoopSubdivision")

add_rules("mode.debug", "mode.release")
add_requires("glfw", "glm")

target("lsdemo")
    set_languages("c99", "cxx17")
    set_kind("binary")
    set_warnings("all")

    add_includedirs("src")
    add_includedirs("src/3rdparty/glad/include")
    add_files("src/**.c", "src/**.cpp")
    add_packages("glfw", "glm")

    after_build(function (target)
        os.cp(target:targetfile(), "bin/")
        os.cp("src/shader", "bin/")
    end)
target_end()
