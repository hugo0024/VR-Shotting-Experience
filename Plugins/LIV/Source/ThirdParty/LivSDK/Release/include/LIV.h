/**
 * \file LIV.h
 * \brief Main entry point of the LIV SDK
 * \copyright 2020-2021 LIV Inc. S.r.o.
 *
 * This is the one and only file you actually need to include to use the LIV SDK.
 * It contains the API of the LIV Loader, and it will include all the necessary files to the C API,
 * data-structures and utility functions you'll need.
 */

#ifndef __LIV_H
#define __LIV_H

#include "LIV_Dynamic.h"
#include "LIV_CAPI.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif

	/// <summary>
	/// Load LIV Bridge functions
	/// </summary>
	/// 
	/// <returns>
	/// Is true upon success. Call liv_get_error() to know what went wrong otherwise.
	/// </returns>
	/// <remarks>This call is specifically not thread safe.
	/// A race condition can occur with other code invoking the dynamic loader of the operating system
	/// (LoadLibrary() or dlopen()). Exact behavior of this call is platform dependent.
	/// </remarks>
	LIV_DECLSPEC bool LIV_Load();

	/// <summary>
	/// Cleanup LIV Bridge functions
	/// </summary>
	/// 
	/// <remarks>
	/// This call is not thread safe.
	/// Calling this function without having called LIV_Load(), and received a successful state is undefined behavior.
	/// This call invalidates internal state of the LIV loader. Calling other LIV functions after this will fail,
	/// unless you call LIV_Load() again.
	/// </remarks>
	LIV_DECLSPEC void LIV_Unload();

	/// <summary>
	/// Get error message describing why something went wrong in the loader.
	/// This also "clear" the error state to "no error" after call.
	///
	/// The returned string is static and does't need to be freed. 
	/// </summary>
	/// <returns>
	/// A null terminated character string describing the error. You can log that!
	/// </returns>
	LIV_DECLSPEC const char* LIV_GetError();

	/// <summary>
	/// Get a 64bit tag from a string that the LIV Bridge recognize
	///
	/// Useful string tags:
	/// - `"BGCTEX"` Background Compositor texture
	/// - `"FGCTEX"` Foreground Compositor texture
	/// </summary>
	/// 
	/// <param name="str">string identifier</param>
	/// <returns>The corresponding tag</returns>
	LIV_DECLSPEC uint64_t LIV_Tag(const char* str);

	//TODO document these better:
	
	/// <summary>
	/// Get the LIV Feature version string
	/// </summary>
	/// 
	/// <returns>Static string, do not free it.</returns>
	LIV_DECLSPEC const char* LIV_GetFeatureVersion();

	/// <summary>
	/// Get the LIV Native SDK version string
	/// </summary>
	/// 
	/// <returns>Static string, do not free it</returns>
	LIV_DECLSPEC const char* LIV_GetCSDKVersion();

	/// <summary>
	/// Debug feature for development. Tell the LIV app that the current process should be manually captured.
	/// Useful when integrating LIV to a environment/level editor. **Do not use in a game release**
	/// </summary>
	LIV_DECLSPEC void LIV_RequestCapture();
	
#ifdef __cplusplus
} // extern "C"
#endif

#endif // __LIV_H

//TODO This is hard to edit and maintain move this to an external markdown file or something similar
/** \mainpage LIV Native SDK
 *
 * \section explainer LIV MR Rendering 
 *
 * The LIV SDK permit your application to leverage the [LIV App](https://store.steampowered.com/app/755540/LIV/)
 * to let it capture your virtual reality application form an "external observer" viewpoint and let users composite
 * themselves inside the virtual environments you created. The typical result looks "as if" the user was *inside*
 * the virtual world, filmed from a 3rd person camera.
 *
 * It provide your users with many options of composition, from professional green screen setups to using a
 * smartphone camera and machine learning to using 3D virtual avatars.
 *
 * This SDK is "XR Runtime agnostic", and can be used with OculusVR, SteamVR and OpenXR headsets running on a
 * Windows PC where the LIV App has been installed.
 *
 * [This Youtube trailer](https://www.youtube.com/embed/i9jpplg6ja4) is one example showing many of these
 * features.
 *
 * \subsection rendering_concept How does MR rendering works?
 *
 * At a high level, the LIV SDK provide you with 3 main things you will need to properly target the LIV
 * application to render:
 *
 * - The configuration of a Camera that is looking at the user in the VR tracked space.
 * This may match an actual video camera, or be totally virtual, and change at any frame
 * - The description of one or multiple "clipping planes" that are used to cut the stage geometry between a
 * "foreground" and a "background" view. This is required for the proper "video sandwich" to be composited
 * by he LIV app
 * - An interface to submit rendered textures to the LIV application.
 *
 * While a compatible application is being captured by the LIV application, it is expected, for each render
 * frame to:
 * 1. Obtain the camera and clip plane "current" state
 * 2. Clear with transparent black (RGBA(0,0,0,0)) both background and foreground render targets and their depth
 * buffers
 * 3. Render a "background" render target containing everything from zFar to zNear
 * 4. Render "clipping planes" in the depth channel of the foreground render target
 * 5. Issue the same render made for the background view, but in the foreground view. The result should be a
 * fully transparent texture where only appear pixels located between
 *
 * \section sdkdoc LIV SDK Documentation
 *
 * \subsection apiconv API conventions
 *
 * - Prefix `LIV_` for functions, enums and data structures
 * - Application must call **once** `LIV_Load()` before calling any LIV api functions. This call must succeed
 * (return true) to be able to call any other LIV function.
 * - When all work is done, call `LIV_Unload()`. This will release the loaded shared object. All code to LIV APIs
 * are now invalid, unless you call `LIV_Load()` again.
 *
 * In case of failure, `LIV_GetError()` returns a string describing the issue found during loading. Cause may be
 * outdated LIV app, missing LIV, LIV app never ran by user.
 *
 * \subsection naming Naming conventions
 *
 * - **Avatar** : A humanoid articulated mesh that can take the place of the user in the environment instead
 * of a camera feed
 * - **Bridge**: The communication layer between the LIV SDK and the LIV App
 * - **Compositor**: The component of the LIV App that process videos and output the result on the user's screen
 * - **Field of view (aka FoV)**: The ***vertical*** opening angle of a camera
 * - **Frame**: All of the resources and state that refers to one single frame of rendering
 * - **InputFrame**: Bidirectional communication data-structure that contains the atomic state of a frame.
 * (See LIV_UpdateInputFrame())
 * - **LIV Space**: The coordinate space that LIV operates in
 * - **Pose**: A position/rotation in a coordinate space
 * - **Stage**: The reference space where the XR system tracking position things
 * - **Transform**: A position/rotation/scale in a coordinate space
 * - **VIVR**: The component of the LIV App that renders additional 3D objects that can be visible in the final
 * composite, including 3D Avatars, the calibration "in VR" UI, and other things from the LIV platform
 * 
 * \subsection coordspace LIV Coordinate space
 *
 * For LIV, the base unit of length is 1 meter. LIV do not deal with rotation angles, but uses notation derived from
 * trigonometric functions (Quaternions and rotation matrices.)
 * 
 * For historic reasons, LIV externally uses a coordinate space that is aligned with the Unity game engine.
 * This means that LIV expresses vectors, quaternions and matrices using Unity's conventions.
 *
 * Specifically, LIV Space is a left-handed cartesian coordinate space, with the Y axis oriented upwards, the X axis
 * to the right, and the Z axis forward.
 *
 * To facilitate integrations, helpers are provided to convert these into standard coordinates. See LIV_GL.h
 * documentation for OpenGL. The LIV conventions matches the default (left handed) one for Direct3D.
 * 
 * \section integration_guide Integration guide
 *
 * The first need you need to do when integrating the LIV SDK in a project is to call LIV_Load().
 * This will cause your program to dynamically load a module that is installed by the LIV App. It will fail if the
 * user hasn't installed (and maybe ran) the LIV Application for the first time.
 * 
 * ```c
 * if(LIV_Load())
 * {
 *     printf("LIV SDK initialized\n");
 * }
 * else
 * {
 *     fprintf(stderr, "LIV SDK failed initialization because : %s\n", LIV_GetError());
 * }
 * ```
 *
 * Anything allocated needs to be cleaned-up properly. This is true for the LIV SDK too. To do so, call LIV_Unload()
 *
 * ```c
 * LIV_Unload();
 * ```
 *
 * The next order of business is to initialize the graphics API you will use. You need to give access to these graphics
 * resources to LIV too.
 *
 * ```c
 * LIV_D3D11_Init(pD3d11Device);
 * ```
 *
 * > **Note**: If you are using OpenGL, you need to have your OpenGL context already created, then you can call
 * > ```c
 * > LIV_GL_Init();
 * > ```
 * > You need to have included the LIV_GL.h file to get access to this call.
 *
 * \subsection input_frame Input Frames
 *
 * To get data in and out of LIV, you need an InputFrame. These are passed through the bridge, consumed, and reallocated by LIV.
 *
 * You can get one by calling LIV_AllocateInputFrame();
 * ```c
 * LIV_InputFrame* inputFrame = LIV_AllocateInputFrame();
 * ```
 *
 * You can release it by calling LIV_FreeInputFrame()
 * ```c
 * LIV_FreeInputFrame(inputFrame);
 * ```
 *
 * ```c
 * LIV_GetInputFrame(inputFrame);
 * ```
 *
 * \subsection rtt Render To Texture setup
 *
 * You need to be able to render a camera view to a texture and submit it to LIV. The characteristics of the texture. Once you have obtained
 * a populated LIV_InputFrame.
 *
 * All the data required to create the textures for rendering, and to configure the cameras for rendering, can be found in LIV_InputFrame::pose.
 * The returned LIV_Pose contains these interesting members:
 *
 * - LIV_Pose::width : width of render texture (in pixels)
 * - LIV_Pose::height : height of render texture (in pixels)
 * - LIV_Pose::verticalFieldOfView : Camera's FOV (in degrees)
 * - LIV_Pose::nearClipPlane : Z depth of clear plane (in meters)
 * - LIV_Pose::farClipPlane : Z depth of the far plane (in meters)
 * - LIV_Pose::local_position : 3D position in LIV Space of the MR camera
 * - LIV_Pose::local_rotation : 3D rotation in LIV Space of the MR camera
 *
 * For example (in OpenGL), we gonna create a pair of texture, depth buffer (for rendering) and framebuffers objects to render the foreground and
 * background textures:
 *
 * ```c
 * GLuint mr_render_texture[2];
 * GLuint mr_depth[2];
 * GLuint mr_fbo[2];
 * bool fb_status[2] = { false, false };
 *
 * glGenTextures(2, mr_render_texture);
 * glGenTextures(2, mr_depth);
 * glGenFramebuffers(2, mr_fbo);
 *
 * for(size_t i = 0; i < 2; ++i)
 * {
 *     //framebuffer
 *     glBindFramebuffer(GL_FRAMEBUFFER, mr_fbo[i]);
 *
 *     //color buffer (the one we will submit to LIV)
 *     glBindTexture(GL_TEXTURE_2D, mr_render_texture[i]);
 *     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
 *     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
 *     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 *     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 *     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, inputFrame.pose.width, inputFrame.pose.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
 *     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mr_render_texture[i], 0);
 *     //depth buffer
 *     glBindTexture(GL_TEXTURE_2D, mr_fbo[i]);
 *     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 *     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 *     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 *     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 *     glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
 *     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mr_depth[i], 0);
 *
 *     fb_status[i] = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
 * }
 *
 * glBindTexture(GL_TEXTURE_2D, 0);
 * glBindFramebuffer(GL_FRAMEBUFFER, 0);
 * ```
 *
 * In Direct3D 11, We will do the equivalent thing: Creating a color and a depth texture, and creating views to these textures like in this snippet
 *
 *```cpp
 *		D3D11_VIEWPORT viewport{};
 *		ID3D11Texture2D* color[2] = {nullptr, nullptr};
 *		ID3D11Texture2D* depth[2] = {nullptr, nullptr};
 *		ID3D11RenderTargetView* renderTargetView[2] = {nullptr, nullptr};
 *		ID3D11DepthStencilView* depthStencilView[2] = {nullptr, nullptr};
 *		UINT w{inputframe.pose.width}, h{inputFrame.pose.height};
 *	
 *		viewport.Width = w;
 *		viewport.Height = h;
 *		viewport.MaxDepth = 1;
 *
 *		for(size_t i = 0; i < 2; ++i)
 *		{
 *			D3D11_TEXTURE2D_DESC texture2DDesc{};
 *			texture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
 *			texture2DDesc.ArraySize = 1;
 *			texture2DDesc.SampleDesc.Count = 1;
 *			texture2DDesc.SampleDesc.Quality = 0;
 *			texture2DDesc.CPUAccessFlags = 0;
 *			texture2DDesc.Width = w;
 *			texture2DDesc.Height = h;
 *			texture2DDesc.MipLevels = 1;
 *			texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
 *			texture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
 *			texture2DDesc.MiscFlags = 0;
 *			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
 *			renderTargetViewDesc.Format = texture2DDesc.Format;
 *			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
 *			renderTargetViewDesc.Texture2D.MipSlice = 0;
 *			if (FAILED(pDevice->CreateTexture2D(&texture2DDesc, nullptr, &color[i])))
 *				printf("Failed to create a color texture of size %dx%d\n", w, h);
 *			if (FAILED(pDevice->CreateRenderTargetView(color[i], &renderTargetViewDesc, &renderTargetView[i])))
 *				fprintf(stderr, "Failed to create RenderTargetView %dx%d\n", w, h);
 *
 *			texture2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
 *			texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
 *			D3D11_DEPTH_STENCIL_VIEW_DESC dephtStencilViewDesc{};
 *			dephtStencilViewDesc.Format = texture2DDesc.Format;
 *			dephtStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
 *			dephtStencilViewDesc.Texture2D.MipSlice = 0;
 *			if (FAILED(pDevice->CreateTexture2D(&texture2DDesc, nullptr, &depth[i])))
 *				fprintf(stderr, "Failed to create a depth stencil texture of size %dx%d\n", w, h);
 *			if (FAILED(pDevice->CreateDepthStencilView(depth[i], &dephtStencilViewDesc, &depthStencilView[i])))
 *				fprintf(stderr, "Failed to create RenderTargetView %dx%d\n", w, h);
 *		}
 * ```
 *
 * Using the LIV_Pose::verticalFieldOfView, LIV_Pose::nearClipPlane, and LIV_Pose::farClipPlane, you can compute the
 * correct projection matrix for your rendering setup.
 *
 * The LIV_Pose::local_position and LIV_Pose::local_rotation, give view the position and rotation of the camera in
 * LIV Space. You can compute the inverse view matrix of the camera using these parameters.
 *
 * These parameters also generally can be directly fed into the "camera" abstraction used by the rendering engine instead
 * of being used to compute matrices used by shader programs during rendering.
 *
 * Example with GLM for OpenGL:
 * 
 * ```cpp
 * const glm::mat4 liv_projection = glm::perspective(glm::radians(inputFrame->pose.verticalFieldOfView), (float)LIVBuffer->w / (float)LIVBuffer->h, inputFrame->pose.nearClipPlane, inputFrame->pose.farClipPlane);
 *
 * const LIV_Vector3 liv_position_gl LIV_Vector3_GL(input_frame->pose.local_position);
 * const LIV_Quaternion liv_rotation_gl LIV_Quaternion_GL(input_frame->pose.local_rotation);
 *
 * //Note: We can do this because LIV vectors and GLM vectors have the same memory layout:
 * const glm::vec3 liv_position;
 * const glm::quat liv_rotation;
 * memcpy(&liv_position, liv_position_gl.data, sizeof(float) * 3);
 * memcpy(liv_rotation, liv_rotaiton_gl.data, sizeof(float) * 4);
 *
 * const auto liv_view_matrix = glm::inverse(glm::translate(glm::mat4(1.f), liv_position) * glm::mat4_cast(liv_rotation));
 * ```
 *
 * Example with DirectXMath:
 * ```cpp
 * const auto aspect = FLOAT(LIVBuffer[0].w) / FLOAT(LIVBuffer[0].h);
 * const auto fov = XMConvertToRadians(inputFrame->pose.verticalFieldOfView);
 * const XMMATRIX liv_projection_matrix = XMMatrixPerspectiveFovLH(fov, aspect, inputFrame->pose.nearClipPlane, inputFrame->pose.farClipPlane);
 *
 * //Camera parameter :
 * const auto p = inputFrame->pose.local_position;
 * const auto q = inputFrame->pose.local_rotation;
 * XMVECTOR liv_quat{};
 * liv_quat.m128_f32[0] = q.x;
 * liv_quat.m128_f32[1] = q.y;
 * liv_quat.m128_f32[2] = q.z;
 * liv_quat.m128_f32[3] = q.w;
 * XMMATRIX liv_cam_transform = XMMatrixMultiply(
 *     XMMatrixRotationQuaternion(liv_quat),
 *     XMMatrixTranslation(p.x, p.y, p.z));
 * XMMATRIX liv_view_matrix = XMMatrixInverse(nullptr,
 *     liv_cam_transform
 * );
 * ```
 * 
 * \image html frustum_and_clip_planes.png
 */