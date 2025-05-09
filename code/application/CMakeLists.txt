#-------------------------------------------------------------------------------
# Game
#-------------------------------------------------------------------------------

nebula_begin_module(application)
    nebula_add_blueprints()
    target_include_directories(application PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    fips_deps(foundation audio resource scripting memdb imgui input options)
    fips_dir(.)
    if (FIPS_WINDOWS)
        fips_files(application.natvis)
    endif ()
        fips_files(
            application/stdneb.h
        )
    fips_dir(game)
        fips_files(
            api.h
            api.cc
			filter.cc
			filter.h
			frameevent.cc
			frameevent.h
			component.cc
			component.h
			componentid.h
			processor.cc
			processor.h
			processorid.h
			dataset.cc
			dataset.h
            category.h
            category.cc
            entity.h
            entity.cc
            entitypool.h
            entitypool.cc
            featureunit.h
            featureunit.cc
            gameserver.h
            gameserver.cc
            manager.h
            manager.cc
            componentserialization.h
            componentserialization.cc
            componentinspection.h
            componentinspection.cc
            world.h
            world.cc
            editorstate.h
            editorstate.cc
        )
    fips_dir(game/messaging)
        fips_files(
            message.h
            message.cc
        )
    fips_dir(basegamefeature)
        fips_files(
            basegamefeatureunit.h
            basegamefeatureunit.cc
            level.h
            level.cc
            levelparser.h
            levelparser.cc
        )
        fips_dir(basegamefeature/managers)
            fips_files(
                blueprintmanager.h
                blueprintmanager.cc
                timemanager.h
                timemanager.cc
            )
        fips_dir(basegamefeature/components)
			fips_files (
				position.h
				position.cc
				orientation.h
				orientation.cc
				scale.h
				scale.cc
				velocity.h
				velocity.cc
			)
            nebula_idl_compile(basegamefeature.json)
    fips_dir(appgame)
        fips_files(
            gameapplication.h
            gameapplication.cc
        )
    fips_dir(.)

nebula_flatc(SYSTEM game/level.fbs)
nebula_end_module()

if(FIPS_WINDOWS)
target_link_options(application PUBLIC "/WHOLEARCHIVE:application")
endif()
target_precompile_headers(application PRIVATE [["foundation/stdneb.h"]] [["application/stdneb.h"]])
