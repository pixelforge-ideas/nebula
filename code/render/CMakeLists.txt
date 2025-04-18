#-------------------------------------------------------------------------------
# Render
#-------------------------------------------------------------------------------

FIND_PROGRAM(SHADERC
       NAMES anyfxcompiler.exe anyfxcompiler
       PATHS ${FIPS_DEPLOY_DIR}/fips-anyfx/anyfxcompiler-windows ${FIPS_DEPLOY_DIR}/fips-anyfx/anyfxcompiler-linux
       NO_DEFAULT_PATH
       )

nebula_begin_module(render)
    add_nebula_shaders()
    set(target_has_shaders 1)
    target_include_directories(render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CODE_ROOT}/addons ${CODE_ROOT}/render)
    fips_deps(foundation input resource anyfx imgui options)
    if (TARGET system_resources-res)
        target_link_libraries(render $<TARGET_OBJECTS:system_resources-res>)
    endif()

    target_precompile_headers(render PRIVATE "render/stdneb.h" "coregraphics/shaderserver.h" "../foundation/core/rtti.h" "graphics/graphicsserver.h" )
     
    if(N_USE_VULKAN)
        fips_deps(glfw3) 
    endif()
     
    fips_files(render/precompiled.h render/stdneb.h) 
      
IF(FALSE) # incrementally enable modules as they get written
        fips_dir(animation)
            fips_files(
                animeventhandlerbase.cc
                animeventhandlerbase.h
                animeventinfo.h
                animeventserver.cc
                animeventserver.h
                animjob.cc
                animjob.h
                animjobenqueuemode.h
                animsequencer.cc
                animsequencer.h
                playclipjob.cc
                playclipjob.h
            )
        fips_dir(apprender)
            fips_files(
                renderapplication.cc
                renderapplication.h
                viewerapplication.cc
                viewerapplication.h
            )
ENDIF()
        fips_dir(characters)
            fips_files(
                charactercontext.cc
                charactercontext.h
                nskfileformatstructs.h
                skeleton.cc
                skeleton.h
                skeletonevaljob.cc
                skeletonjoint.h
                skeletonloader.cc
                skeletonloader.h
                skeletonresource.cc
                skeletonresource.h
            )

        fips_dir(clustering)
            fips_files(
                clustercontext.cc
                clustercontext.h
                clusterutil.h
            )

        fips_dir(coreanimation)
            fips_files(
                animation.cc
                animation.h
                animationloader.cc
                animationloader.h
                animationresource.cc
                animationresource.h
                animclip.cc
                animclip.h
                animcurve.h
                animevent.h
                animeventemitter.cc
                animeventemitter.h
                animkeybuffer.cc
                animkeybuffer.h
                animsamplebuffer.cc
                animsamplebuffer.h
                animsamplejob.cc
                animsamplemask.h
                animsamplemixinfo.h
                #animutil.cc
                #animutil.h
                curvetype.cc
                curvetype.h
                infinitytype.cc
                infinitytype.h
                naxfileformatstructs.h
                sampletype.cc
                sampletype.h
            )
        fips_dir(coregraphics/base GROUP "coregraphics/base")
            fips_files(
                displaydevicebase.cc
                displaydevicebase.h
                #renderdevicebase.cc
                #renderdevicebase.h
                shaderserverbase.cc
                shaderserverbase.h
                shaperendererbase.cc
                shaperendererbase.h
                textrendererbase.cc
                textrendererbase.h
            )
        fips_dir(coregraphics)
            fips_files(
                accelerationstructure.h
                adapter.cc
                adapter.h
                adapterinfo.h
                antialiasquality.cc
                antialiasquality.h
                barrier.h
                buffer.cc
                buffer.h
                gpubuffertypes.h
                commandbuffer.cc
                commandbuffer.h
                config.h
                displaydevice.cc
                displaydevice.h
                displayevent.h
                displayeventhandler.cc
                displayeventhandler.h
                displaymode.h
                drawthread.cc
                drawthread.h
                event.h
                fence.h
                graphicsdevice.h
                image.cc
                image.h
                imagefileformat.cc
                imagefileformat.h
                indextype.h
                memory.cc
                memory.h
                mesh.cc
                mesh.h
                meshloader.cc
                meshloader.h
                meshresource.cc
                meshresource.h
                nvx3fileformatstructs.h
                pass.h
                pinnedbuffer.h
                pipeline.h
                pixelformat.cc
                pixelformat.h
                primitivegroup.h
                primitivetopology.cc
                primitivetopology.h
                renderevent.h
                rendereventhandler.cc
                rendereventhandler.h
                rendershape.cc
                rendershape.h
                resourcetable.cc
                resourcetable.h
                sampler.h
                semaphore.h
                shader.cc
                shader.h
                shaderfeature.cc
                shaderfeature.h
                shaderidentifier.cc
                shaderidentifier.h
                shadersemantics.h
                shaderserver.cc
                shaderserver.h
                shaperenderer.cc
                shaperenderer.h
                sparsebuffer.h
                shaderloader.cc
                shaderloader.h
                streamtexturesaver.h
                swapchain.h
                textelement.cc
                textelement.h
                textrenderer.cc
                textrenderer.h
                texture.cc
                texture.h
                textureloader.cc
                textureloader.h
                textureview.h
                vertexcomponent.h
                vertexlayout.cc
                vertexlayout.h
                window.h
            )
        fips_dir(coregraphics/debug GROUP "coregraphics/debug")
            fips_files(
                displaypagehandler.cc
                displaypagehandler.h
                meshpagehandler.cc
                meshpagehandler.h
                shaderpagehandler.cc
                shaderpagehandler.h
                texturepagehandler.cc
                texturepagehandler.h
            )

        fips_dir(coregraphics/glfw GROUP "coregraphics/glfw")
            fips_files(
                glfwdisplaydevice.cc
                glfwdisplaydevice.h
                glfwwindow.cc
                glfwwindow.h
            )
        fips_dir(coregraphics/legacy GROUP "coregraphics/legacy")
            fips_files(
                nvx2fileformatstructs.h
                nvx2streamreader.cc
                nvx2streamreader.h
            )
        fips_dir(coregraphics/load GROUP "coregraphics/load")
            fips_files(glimltypes.h
                   glimltypes.cc
                   )
    if (N_USE_D3D11)
        fips_dir(coregraphics/base GROUP "coregraphics/d3d11")
            fips_files(
                d3d11barrier.cc
                d3d11barrier.h
                d3d11buffer.cc
                d3d11buffer.h
                d3d11bufferlock.cc
                d3d11bufferlock.h
                d3d11displaydevice.cc
                d3d11displaydevice.h
                d3d11event.cc
                d3d11event.h
                d3d11feedbackbuffer.cc
                d3d11feedbackbuffer.h
                d3d11mesh.cc
                d3d11mesh.h
                d3d11pass.cc
                d3d11pass.h
                d3d11renderdevice.cc
                d3d11renderdevice.h
                d3d11rendertexture.cc
                d3d11rendertexture.h
                d3d11resource.cc
                d3d11resource.h
                d3d11shader.cc
                d3d11shader.h
                d3d11shaderserver.cc
                d3d11shaderserver.h
                d3d11shaderstate.cc
                d3d11shaderstate.h
                d3d11shadervariable.cc
                d3d11shadervariable.h
                d3d11shadervariation.cc
                d3d11shadervariation.h
                d3d11shaperenderer.cc
                d3d11shaperenderer.h
                d3d11streamtexturesaver.cc
                d3d11streamtexturesaver.h
                d3d11textrenderer.cc
                d3d11textrenderer.h
                d3d11texture.cc
                d3d11texture.h
                d3d11vertexcomponent.h
                d3d11vertexlayout.cc
                d3d11vertexlayout.h
                d3d11vertexlayoutserver.cc
                d3d11vertexlayoutserver.h
                d3d11window.cc
                d3d11window.h
            )
    elseif (N_USE_VULKAN)
        fips_dir(coregraphics/vk GROUP "coregraphics/vk")
            fips_files(
                vkaccelerationstructure.cc
                vkaccelerationstructure.h
                vkbarrier.cc
                vkbarrier.h
                vkbuffer.cc
                vkbuffer.h
                vkcommandbuffer.cc
                vkcommandbuffer.h
                vkcommandbufferthread.cc
                vkcommandbufferthread.h
                vkevent.cc
                vkevent.h
                vkfence.cc
                vkfence.h
                vkgraphicsdevice.cc
                vkgraphicsdevice.h
                vkloader.cc
                vkloader.h
                vkmemory.cc
                vkmemory.h
                vkpass.cc
                vkpass.h
                vkpipeline.cc
                vkpipeline.h
                vkpipelinedatabase.cc
                vkpipelinedatabase.h
                vkresourcetable.cc
                vkresourcetable.h
                vksampler.cc
                vksampler.h
                vksemaphore.cc
                vksemaphore.h
                vkshader.cc
                vkshader.h
                vkshaderprogram.cc
                vkshaderprogram.h
                vkshaderserver.cc
                vkshaderserver.h
                vkshaperenderer.cc
                vkshaperenderer.h
                vksubcontexthandler.cc
                vksubcontexthandler.h
                vkswapchain.cc
                vkswapchain.h
                vktextrenderer.cc
                vktextrenderer.h
                vktexture.cc
                vktexture.h
                vktextureview.cc
                vktextureview.h
                vktypes.cc
                vktypes.h
                vkvertexlayout.cc
                vkvertexlayout.h
            )
    endif()
        fips_dir(debug)
            fips_files(
                framescriptinspector.cc
                framescriptinspector.h
            )
        fips_dir(decals)
            fips_files(
                decalcontext.cc
                decalcontext.h
            )
        fips_dir(fog)
            fips_files(
                volumetricfogcontext.cc
                volumetricfogcontext.h
            )
        fips_dir(frame)
            nebula_framescript_add(default.json shadows.json) 
            #fips_files(
            #    framebarrier.cc
            #    framebarrier.h
            #    framebatchtype.h
            #    frameblit.cc
            #    frameblit.h
            #    framecode.cc
            #    framecode.h
            #    framecompute.cc
            #    framecompute.h
            #    frameplugin.cc
            #    frameplugin.h
            #    frameresolve.cc
            #    frameresolve.h
            #    framecopy.cc
            #    framecopy.h
            #    #frameevent.cc
            #    #frameevent.h
            #    framemipmap.cc
            #    framemipmap.h
            #    frameop.cc
            #    frameop.h
            #    framepass.cc
            #    framepass.h
            #    framescript.cc
            #    framescript.h
            #    framescriptloader.cc
            #    framescriptloader.h
            #    framesubgraph.cc
            #    framesubgraph.h
            #    frameserver.cc
            #    frameserver.h
            #    framesubmission.cc
            #    framesubmission.h
            #    framesubpass.cc
            #    framesubpass.h
            #    framesubpassplugin.cc
            #    framesubpassplugin.h
            #    framesubpassbatch.cc
            #    framesubpassbatch.h
            #    framesubpassfullscreeneffect.cc
            #    framesubpassfullscreeneffect.h
            #    framesubpassorderedbatch.cc
            #    framesubpassorderedbatch.h
            #    framesubpassplugin.cc
            #    framesubpassplugin.h
            #    frameswap.cc
            #    frameswap.h
            #)
        fips_dir(framescripts)
            fips_files(
                framescripts.cc
                framescripts.h
            )
        fips_dir(gi)
            fips_files(
                ddgicontext.cc
                ddgicontext.h
            )
        fips_dir(gi/shaders)
            add_shaders(probe_shared.fxh probe_update.fx probe_finalize.fx probe_debug.fx gi_volume_cull.fx probe_relocate_and_classify.fx)
        fips_dir(graphics) 
            fips_files(
				bindlessregistry.cc
				bindlessregistry.h
                cameracontext.cc
                cameracontext.h
                camerasettings.cc
                camerasettings.h
                environmentcontext.cc
                environmentcontext.h
				globalconstants.cc
				globalconstants.h
                graphicscontext.cc
                graphicscontext.h
                graphicsentity.h
                graphicsserver.cc
                graphicsserver.h
                stage.cc
                stage.h
                view.cc
                view.h
            )
        fips_dir(graphics/glfw)
                fips_files(
                    glfwgraphicsdisplayeventhandler.h
                    glfwgraphicsdisplayeventhandler.cc
                )

        fips_dir(input/glfw GROUP "input/glfw")
            fips_files(
                glfwinputdisplayeventhandler.cc
                glfwinputdisplayeventhandler.h
            )

    fips_dir(lighting)
        fips_files(
            csmutil.cc
            csmutil.h
            lightcontext.cc
            lightcontext.h
            lightprobecontext.cc
            lightprobecontext.h
            ltc.h
        )
IF(FALSE)

        fips_dir(instancing)
            fips_files(
                instancerenderer.cc
                instancerenderer.h
                instanceserver.cc
                instanceserver.h
            )
        fips_dir(instancing/base GROUP "instancing/base")
            fips_files(
                instancerendererbase.cc
                instancerendererbase.h
                instanceserverbase.cc
                instanceserverbase.h
            )
    if (N_USE_VULKAN)
        fips_dir(instancing/vk GROUP "instancing/vk")
            fips_files(
                vkinstancerenderer.cc
                vkinstancerenderer.h
                vkinstanceserver.cc
                vkinstanceserver.h
            )
    elseif (N_USE_D3D11)
        fips_dir(instancing/d3d11 GROUP "instancing/d3d11")
            fips_files(
                d3d11instancerenderer.cc
                d3d11instancerenderer.h
                d3d11instanceserver.cc
                d3d11instanceserver.h
            )
    endif()
        fips_dir(jobs GROUPS "jobs")
            fips_files(
            ../coreanimation/jobs/animjobsample.cc
            ../coreanimation/jobs/animjobsamplemix.cc
            ../coreanimation/jobs/animjobutil.h
            ../skinning/jobs/charevalskeletonjob.cc
            #../visibility/jobs/visibilityboxsystemjob.cc
            #../visibility/jobs/visibilityquadtreejob.cc
            ../particles/jobs/particlejob.cc
            )

        fips_dir(lighting/handlers GROUP "lighting/handlers")
            fips_files(
                abstractlightentityhandler.cc
                globallightentityhandler.cc
                pointlightentityhandler.cc
                spotlightentityhandler.cc
            )
ENDIF()
        fips_dir(materials)
			# Add engine templates and generate glue file which is needed by the material system
			nebula_material_template_add(base.json gltf.json)
            nebula_project_material_template_add()

            fips_files(
                material.cc
                material.h
                materialloader.cc
                materialloader.h
                materialtemplatetypes.h
                materialvariant.h
                shaderconfig.cc
                shaderconfig.h
            )

        fips_dir(models/nodes GROUP "models/nodes")
            fips_files(
                characternode.cc
                characternode.h
                characterskinnode.cc
                characterskinnode.h
                modelnode.cc
                modelnode.h
                particlesystemnode.cc
                particlesystemnode.h
                primitivenode.cc
                primitivenode.h
                shaderstatenode.cc
                shaderstatenode.h
                transformnode.cc
                transformnode.h
            )
        fips_dir(models)
            fips_files(
                model.cc
                model.h
                modelcontext.cc
                modelcontext.h
                modelloader.cc
                modelloader.h
            )

        fips_dir(raytracing)
        fips_files(
            raytracingcontext.cc
            raytracingcontext.h
        )
        fips_dir(raytracing/shaders)
            add_shaders(raytracetest.fx gltfhit.fx brdfhit.fx bsdfhit.fx particlehit.fx light_grid_cs.fx terrainhit.fx)

        fips_dir(renderutil)
        fips_files(
            drawfullscreenquad.cc
            drawfullscreenquad.h
            freecamerautil.cc
            freecamerautil.h
            geometryhelpers.cc
            geometryhelpers.h
            mayacamerautil.cc
            mayacamerautil.h
            mouserayutil.cc
            mouserayutil.h
        )
        fips_dir(particles)
            fips_files(
                emitterattrs.cc
                emitterattrs.h
                emittermesh.cc
                emittermesh.h
                envelopecurve.cc
                envelopecurve.h
                envelopesamplebuffer.cc
                envelopesamplebuffer.h
                particle.h
                particlecontext.cc
                particlecontext.h
                particlejob.cc
                particlerenderinfo.h
            )

    IF(FALSE)
        fips_dir(picking)
            fips_files(
                pickingserver.cc
                pickingserver.h
            )



ENDIF()
        fips_dir(posteffects)
            fips_files(
                bloomcontext.cc
                bloomcontext.h
                downsamplingcontext.cc
                downsamplingcontext.h
                histogramcontext.cc
                histogramcontext.h
                ssaocontext.cc
                ssaocontext.h
                ssrcontext.cc
                ssrcontext.h
            )

        fips_dir(terrain)
            fips_files(
                occupancyquadtree.h
                terraincontext.h
                terraincontext.cc
                terrainculljob.cc
                texturetilecache.h
            )
        fips_dir(terrain/shaders)
            add_shaders(
                terrain_include.fxh terrain.fx terrain_tile_write.fx terrain_mesh_generate.fx
            )

        fips_dir(vegetation)
            fips_files(
                vegetationcontext.h
                vegetationcontext.cc
            )
                
        fips_dir(visibility)
            fips_files(
                visibility.h
                visibilitycontext.cc
                visibilitycontext.h
                visibilitydependencyjob.cc
            )
        fips_dir(visibility/systems)
            fips_files(
                boxsystem.h
                boxsystem.cc
                boxsystemjob.cc
                bruteforcesystem.h
                bruteforcesystem.cc
                octreesystem.h
                octreesystem.cc
                octreesystemjob.cc
                portalsystem.h
                portalsystem.cc
                portalsystemjob.cc
                quadtreesystem.h
                quadtreesystem.cc
                quadtreesystemjob.cc
                visibilitysystem.h
                visibilitysystem.cc
            )

nebula_end_module()


