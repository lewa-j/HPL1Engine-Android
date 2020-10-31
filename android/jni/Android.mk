LOCAL_PATH:= $(call my-dir)/../..

include $(CLEAR_VARS)

LOCAL_MODULE := hpl1engine
LOCAL_CPP_EXTENSION := .cpp
LOCAL_SRC_FILES := sources/impl/LowLevelGraphicsAndroid.cpp \
sources/impl/LowLevelInputAndroid.cpp \
sources/impl/LowLevelResourcesAndroid.cpp \
sources/impl/LowLevelSystemAndroid.cpp \
sources/impl/LowLevelSystemCommon.cpp \
sources/impl/AndroidBitmap2D.cpp \
sources/impl/AndroidGameSetup.cpp \
sources/impl/AndroidTexture.cpp \
sources/impl/GLSLProgram.cpp \
sources/impl/VertexBufferGLES.cpp \
sources/impl/MeshLoaderCollada.cpp \
sources/impl/MeshLoaderColladaHelpers.cpp \
sources/impl/MeshLoaderColladaLoader.cpp \
sources/impl/SqScript.cpp \
sources/impl/stdstring.cpp \
sources/impl/platform/posix.cpp \
sources/impl/tinyXml/tinystr.cpp sources/impl/tinyXml/tinyxml.cpp sources/impl/tinyXml/tinyxmlerror.cpp sources/impl/tinyXml/tinyxmlparser.cpp \
sources/ai/AI.cpp \
sources/ai/AINodeContainer.cpp \
sources/ai/AINodeGenerator.cpp \
sources/ai/AStar.cpp \
sources/ai/StateMachine.cpp \
sources/game/Game.cpp \
sources/game/GameTypes.cpp \
sources/game/SaveGame.cpp \
sources/game/ScriptFuncs.cpp \
sources/game/Updater.cpp \
sources/graphics/Animation.cpp \
sources/graphics/AnimationTrack.cpp \
sources/graphics/BackgroundImage.cpp \
sources/graphics/Beam.cpp \
sources/graphics/BillBoard.cpp \
sources/graphics/Bone.cpp \
sources/graphics/BoneState.cpp \
sources/graphics/Color.cpp \
sources/graphics/FontData.cpp \
sources/graphics/GfxObject.cpp \
sources/graphics/GPUProgram.cpp \
sources/graphics/Graphics.cpp \
sources/graphics/GraphicsDrawer.cpp \
sources/graphics/ImageEntityData.cpp \
sources/graphics/Material.cpp \
sources/graphics/Material_Additive.cpp \
sources/graphics/Material_Alpha.cpp \
sources/graphics/Material_BaseLight.cpp \
sources/graphics/Material_Bump.cpp \
sources/graphics/Material_BumpColorSpec.cpp \
sources/graphics/Material_BumpSpec.cpp \
sources/graphics/Material_BumpSpec2D.cpp \
sources/graphics/Material_Diffuse.cpp \
sources/graphics/Material_Diffuse2D.cpp \
sources/graphics/Material_DiffuseAdditive2D.cpp \
sources/graphics/Material_DiffuseAlpha2D.cpp \
sources/graphics/Material_DiffuseSpec.cpp \
sources/graphics/Material_EnvMap_Reflect.cpp \
sources/graphics/Material_Fallback01_BaseLight.cpp \
sources/graphics/Material_Fallback02_BaseLight.cpp \
sources/graphics/Material_Flat.cpp \
sources/graphics/Material_FontNormal.cpp \
sources/graphics/Material_Modulative.cpp \
sources/graphics/Material_ModulativeX2.cpp \
sources/graphics/Material_Smoke2D.cpp \
sources/graphics/Material_Water.cpp \
sources/graphics/MaterialHandler.cpp \
sources/graphics/Mesh.cpp \
sources/graphics/Mesh2d.cpp \
sources/graphics/MeshCreator.cpp \
sources/graphics/ParticleEmitter.cpp \
sources/graphics/ParticleEmitter2D.cpp \
sources/graphics/ParticleEmitter3D.cpp \
sources/graphics/ParticleEmitter3D_UserData.cpp \
sources/graphics/ParticleSystem3D.cpp \
sources/graphics/Renderable.cpp \
sources/graphics/Renderer2D.cpp \
sources/graphics/Renderer3D.cpp \
sources/graphics/RendererPostEffects.cpp \
sources/graphics/RenderList.cpp \
sources/graphics/RenderObject2D.cpp \
sources/graphics/RenderState.cpp \
sources/graphics/Skeleton.cpp \
sources/graphics/SubMesh.cpp \
sources/gui/Gui.cpp \
sources/gui/GuiGfxElement.cpp \
sources/gui/GuiMaterialBasicTypes.cpp \
sources/gui/GuiPopUp.cpp \
sources/gui/GuiPopUpMessageBox.cpp \
sources/gui/GuiSet.cpp \
sources/gui/GuiSkin.cpp \
sources/gui/Widget.cpp \
sources/gui/WidgetBaseClasses.cpp \
sources/gui/WidgetButton.cpp \
sources/gui/WidgetCheckBox.cpp \
sources/gui/WidgetComboBox.cpp \
sources/gui/WidgetFrame.cpp \
sources/gui/WidgetImage.cpp \
sources/gui/WidgetLabel.cpp \
sources/gui/WidgetListBox.cpp \
sources/gui/WidgetSlider.cpp \
sources/gui/WidgetTextBox.cpp \
sources/gui/WidgetWindow.cpp \
sources/haptic/Haptic.cpp \
sources/haptic/LowLevelHaptic.cpp \
sources/input/Action.cpp \
sources/input/ActionHaptic.cpp \
sources/input/ActionKeyboard.cpp \
sources/input/ActionMouseButton.cpp \
sources/input/Input.cpp \
sources/input/InputDevice.cpp \
sources/input/Keyboard.cpp \
sources/input/Mouse.cpp \
sources/math/Math.cpp \
sources/math/BoundingVolume.cpp \
sources/math/CRC.cpp \
sources/math/Frustum.cpp \
sources/math/MathTypes.cpp \
sources/math/MeshTypes.cpp \
sources/math/Quaternion.cpp \
sources/physics/Body2D.cpp \
sources/physics/CharacterBody.cpp \
sources/physics/Collider2D.cpp \
sources/physics/Physics.cpp \
sources/physics/PhysicsBody.cpp \
sources/physics/PhysicsController.cpp \
sources/physics/PhysicsJoint.cpp \
sources/physics/PhysicsJointBall.cpp \
sources/physics/PhysicsJointHinge.cpp \
sources/physics/PhysicsJointScrew.cpp \
sources/physics/PhysicsJointSlider.cpp \
sources/physics/PhysicsWorld.cpp \
sources/physics/SurfaceData.cpp \
sources/resources/AnimationManager.cpp \
sources/resources/ConfigFile.cpp \
sources/resources/EntityLoader_Object.cpp \
sources/resources/FileSearcher.cpp \
sources/resources/FontManager.cpp \
sources/resources/FrameBitmap.cpp \
sources/resources/FrameTexture.cpp \
sources/resources/GpuProgramManager.cpp \
sources/resources/ImageEntityManager.cpp \
sources/resources/ImageManager.cpp \
sources/resources/LanguageFile.cpp \
sources/resources/MaterialManager.cpp \
sources/resources/MeshLoaderHandler.cpp \
sources/resources/MeshManager.cpp \
sources/resources/ParticleManager.cpp \
sources/resources/ResourceBase.cpp \
sources/resources/ResourceImage.cpp \
sources/resources/ResourceManager.cpp \
sources/resources/Resources.cpp \
sources/resources/ScriptManager.cpp \
sources/resources/SoundEntityManager.cpp \
sources/resources/SoundManager.cpp \
sources/resources/TextureManager.cpp \
sources/resources/TileSetManager.cpp \
sources/resources/VideoManager.cpp \
sources/scene/AnimationState.cpp \
sources/scene/Area2D.cpp \
sources/scene/Camera2D.cpp \
sources/scene/Camera3D.cpp \
sources/scene/ColliderEntity.cpp \
sources/scene/Entity.cpp \
sources/scene/Entity2D.cpp \
sources/scene/Entity3D.cpp \
sources/scene/GridMap2D.cpp \
sources/scene/ImageEntity.cpp \
sources/scene/Light.cpp \
sources/scene/Light2D.cpp \
sources/scene/Light2DPoint.cpp \
sources/scene/Light3D.cpp \
sources/scene/Light3DPoint.cpp \
sources/scene/Light3DSpot.cpp \
sources/scene/MeshEntity.cpp \
sources/scene/SubMeshEntity.cpp \
sources/scene/MultiImageEntity.cpp \
sources/scene/Node.cpp \
sources/scene/Node2D.cpp \
sources/scene/Node3D.cpp \
sources/scene/NodeState.cpp \
sources/scene/PortalContainer.cpp \
sources/scene/Scene.cpp \
sources/scene/SectorVisibility.cpp \
sources/scene/SoundEntity.cpp \
sources/scene/SoundSource.cpp \
sources/scene/Tile.cpp \
sources/scene/TileData.cpp \
sources/scene/TileLayer.cpp \
sources/scene/TileMap.cpp \
sources/scene/TileMapLineIt.cpp \
sources/scene/TileMapRectIt.cpp \
sources/scene/TileSet.cpp \
sources/scene/World2D.cpp \
sources/scene/World3D.cpp \
sources/sound/LowLevelSound.cpp \
sources/sound/MusicHandler.cpp \
sources/sound/Sound.cpp \
sources/sound/SoundEntityData.cpp \
sources/sound/SoundHandler.cpp \
sources/system/LogicTimer.cpp \
sources/system/SerializeClass.cpp \
sources/system/String.cpp \
sources/system/System.cpp

LOCAL_CPPFLAGS := -std=c++11 -D_USRDLL -DHPL_EXPORTS
LOCAL_CPPFLAGS += -g -ggdb -D_DEBUG
#-DNDEBUG
LOCAL_CPPFLAGS += -Wno-extern-c-compat -Wno-switch -Wno-undefined-var-template -Wno-delete-non-virtual-dtor -Wno-ignored-attributes
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/dependencies
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
LOCAL_EXPORT_LDFLAGS := -lGLESv2
#LOCAL_EXPORT_LDLIBS :=
LOCAL_STATIC_LIBRARIES := android_native_app_glue
LOCAL_THIN_ARCHIVE := true
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := hpl1game
LOCAL_CPP_EXTENSION := .cpp
LOCAL_SRC_FILES := tests/SpotLightTest/SpotLightTest.cpp \
tests/Common/SimpleCamera.cpp
LOCAL_STATIC_LIBRARIES := hpl1engine

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/dependencies

LOCAL_CPPFLAGS := -std=c++11
LOCAL_LDFLAGS :=
#LOCAL_CPPFLAGS += -g -ggdb
#LOCAL_LDFLAGS += -g -ggdb

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
