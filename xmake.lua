
set_version("1.0.0")
-- 启用生成 compile_commands.json
add_rules("mode.debug", "mode.release", "plugin.compile_commands.autoupdate")

target("BuckshotRouletteTool")
    set_languages("c++23")
    add_rules("qt.widgetapp")
    add_frameworks("QtCore", "QtGui", "QtWidgets")
    add_includedirs("src/")
    set_configdir("gen/config")
    add_includedirs("gen/config")
    add_configfiles("version.h.in")
    add_files("src/*.cpp")
    add_files("src/bullettracker.h")
    add_files("src/itemmanager.h") 
    add_files("src/decisionhelper.h")
    add_files("src/bullettypewidget.h")
    add_files("src/main.h")
    add_headerfiles("src/*.h")
    
    -- 添加UTF-8编译选项
    if is_plat("windows") then
        add_cxflags("/utf-8")
    end


    -- 构建后自动调用 windeployqt 部署 Qt 依赖
    after_build(function (target)
        local exepath = path.join(target:targetdir(), target:basename() .. ".exe")
        -- 自动检测 Qt 安装路径
        import("detect.sdks.find_qt")
        local qt = find_qt()
        local windeployqt = qt and qt.bindir and path.join(qt.bindir, "windeployqt.exe")
        if windeployqt and os.isfile(windeployqt) then
            os.vrunv(windeployqt, {exepath})
        else
            print("windeployqt.exe 未找到，请检查 Qt 安装路径或环境变量 QTDIR")
        end
    end)

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

