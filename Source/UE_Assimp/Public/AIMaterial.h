// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AIScene.h"
#include "UE_Assimp.h"
#include "assimp/material.h"
#include "UObject/NoExportTypes.h"
#include "AIMaterial.generated.h"

/**
 * 
 */



// ---------------------------------------------------------------------------
/** @brief Defines how the Nth texture of a specific type is combined with
*  the result of all previous layers.
*
*  Example (left: key, right: value): <br>
*  @code
*  DiffColor0     - gray
*  DiffTextureOp0 - aiTextureOpMultiply
*  DiffTexture0   - tex1.png
*  DiffTextureOp0 - aiTextureOpAdd
*  DiffTexture1   - tex2.png
*  @endcode
*  Written as equation, the final diffuse term for a specific pixel would be:
*  @code
*  diffFinal = DiffColor0 * sampleTex(DiffTexture0,UV0) +
*     sampleTex(DiffTexture1,UV0) * diffContrib;
*  @endcode
*  where 'diffContrib' is the intensity of the incoming light for that pixel.
*/
UENUM(Blueprintable,BlueprintType)

enum EAiTextureOp {
	/** T = T1 * T2 */
	AiTextureOp_Multiply = 0x0,

    /** T = T1 + T2 */
    AiTextureOp_Add = 0x1,

    /** T = T1 - T2 */
    AiTextureOp_Subtract = 0x2,

    /** T = T1 / T2 */
    AiTextureOp_Divide = 0x3,

    /** T = (T1 + T2) - (T1 * T2) */
    AiTextureOp_SmoothAdd = 0x4,

    /** T = T1 + (T2-0.5) */
    AiTextureOp_SignedAdd = 0x5,

#ifndef SWIG
    _AiTextureOp_Force32Bit = INT_MAX
#endif
};


// ---------------------------------------------------------------------------
/** @brief Defines how UV coordinates outside the [0...1] range are handled.
*
*  Commonly referred to as 'wrapping mode'.
*/
UENUM(Blueprintable,BlueprintType)
enum EAiTextureMapMode {
	/** A texture coordinate u|v is translated to u%1|v%1
	*/
	AiTextureMapMode_Wrap = 0x0,

    /** Texture coordinates outside [0...1]
    *  are clamped to the nearest valid value.
    */
    AiTextureMapMode_Clamp = 0x1,

    /** If the texture coordinates for a pixel are outside [0...1]
    *  the texture is not applied to that pixel
    */
    AiTextureMapMode_Decal = 0x3,

    /** A texture coordinate u|v becomes u%1|v%1 if (u-(u%1))%2 is zero and
    *  1-(u%1)|1-(v%1) otherwise
    */
    AiTextureMapMode_Mirror = 0x2,

#ifndef SWIG
    _AiTextureMapMode_Force32Bit = INT_MAX
#endif
};


// ---------------------------------------------------------------------------
/** @brief Defines how the mapping coords for a texture are generated.
*
*  Real-time applications typically require full UV coordinates, so the use of
*  the aiProcess_GenUVCoords step is highly recommended. It generates proper
*  UV channels for non-UV mapped objects, as long as an accurate description
*  how the mapping should look like (e.g spherical) is given.
*  See the #AI_MATKEY_MAPPING property for more details.
*/
UENUM(Blueprintable,BlueprintType)
enum EAiTextureMapping {
	/** The mapping coordinates are taken from an UV channel.
	*
	*  #AI_MATKEY_UVWSRC property specifies from which UV channel
	*  the texture coordinates are to be taken from (remember,
	*  meshes can have more than one UV channel).
	*/
	AiTextureMapping_UV = 0x0,

    /** Spherical mapping */
    AiTextureMapping_SPHERE = 0x1,

    /** Cylindrical mapping */
    AiTextureMapping_CYLINDER = 0x2,

    /** Cubic mapping */
    AiTextureMapping_BOX = 0x3,

    /** Planar mapping */
    AiTextureMapping_PLANE = 0x4,

    /** Undefined mapping. Have fun. */
    AiTextureMapping_OTHER = 0x5,

#ifndef SWIG
    _AiTextureMapping_Force32Bit = INT_MAX
#endif
};


// ---------------------------------------------------------------------------
/** @brief Defines the purpose of a texture
 *
 *  This is a very difficult topic. Different 3D packages support different
 *  kinds of textures. For very common texture types, such as bumpmaps, the
 *  rendering results depend on implementation details in the rendering
 *  pipelines of these applications. Assimp loads all texture references from
 *  the model file and tries to determine which of the predefined texture
 *  types below is the best choice to match the original use of the texture
 *  as closely as possible.<br>
 *
 *  In content pipelines you'll usually define how textures have to be handled,
 *  and the artists working on models have to conform to this specification,
 *  regardless which 3D tool they're using.
 */
UENUM(Blueprintable,BlueprintType)
enum EAiTextureType {
    /** Dummy value.
     *
     *  No texture, but the value to be used as 'texture semantic'
     *  (#aiMaterialProperty::mSemantic) for all material properties
     *  *not* related to textures.
     */
    AiTextureType_NONE = 0,

    /** LEGACY API MATERIALS
     * Legacy refers to materials which
     * Were originally implemented in the specifications around 2000.
     * These must never be removed, as most engines support them.
     */

    /** The texture is combined with the result of the diffuse
     *  lighting equation.
     *  OR
     *  PBR Specular/Glossiness
     */
    AiTextureType_DIFFUSE = 1,

    /** The texture is combined with the result of the specular
     *  lighting equation.
     *  OR
     *  PBR Specular/Glossiness
     */
    AiTextureType_SPECULAR = 2,

    /** The texture is combined with the result of the ambient
     *  lighting equation.
     */
    AiTextureType_AMBIENT = 3,

    /** The texture is added to the result of the lighting
     *  calculation. It isn't influenced by incoming light.
     */
    AiTextureType_EMISSIVE = 4,

    /** The texture is a height map.
     *
     *  By convention, higher gray-scale values stand for
     *  higher elevations from the base height.
     */
    AiTextureType_HEIGHT = 5,

    /** The texture is a (tangent space) normal-map.
     *
     *  Again, there are several conventions for tangent-space
     *  normal maps. Assimp does (intentionally) not
     *  distinguish here.
     */
    AiTextureType_NORMALS = 6,

    /** The texture defines the glossiness of the material.
     *
     *  The glossiness is in fact the exponent of the specular
     *  (phong) lighting equation. Usually there is a conversion
     *  function defined to map the linear color values in the
     *  texture to a suitable exponent. Have fun.
    */
    AiTextureType_SHININESS = 7,

    /** The texture defines per-pixel opacity.
     *
     *  Usually 'white' means opaque and 'black' means
     *  'transparency'. Or quite the opposite. Have fun.
    */
    AiTextureType_OPACITY = 8,

    /** Displacement texture
     *
     *  The exact purpose and format is application-dependent.
     *  Higher color values stand for higher vertex displacements.
    */
    AiTextureType_DISPLACEMENT = 9,

    /** Lightmap texture (aka Ambient Occlusion)
     *
     *  Both 'Lightmaps' and dedicated 'ambient occlusion maps' are
     *  covered by this material property. The texture contains a
     *  scaling value for the final color value of a pixel. Its
     *  intensity is not affected by incoming light.
    */
    AiTextureType_LIGHTMAP = 10,

    /** Reflection texture
     *
     * Contains the color of a perfect mirror reflection.
     * Rarely used, almost never for real-time applications.
    */
    AiTextureType_REFLECTION = 11,

    /** PBR Materials
     * PBR definitions from maya and other modelling packages now use this standard.
     * This was originally introduced around 2012.
     * Support for this is in game engines like Godot, Unreal or Unity3D.
     * Modelling packages which use this are very common now.
     */

    AiTextureType_BASE_COLOR = 12,
    AiTextureType_NORMAL_CAMERA = 13,
    AiTextureType_EMISSION_COLOR = 14,
    AiTextureType_METALNESS = 15,
    AiTextureType_DIFFUSE_ROUGHNESS = 16,
    AiTextureType_AMBIENT_OCCLUSION = 17,

    /** PBR Material Modifiers
    * Some modern renderers have further PBR modifiers that may be overlaid
    * on top of the 'base' PBR materials for additional realism.
    * These use multiple texture maps, so only the base type is directly defined
    */

    /** Sheen
    * Generally used to simulate textiles that are covered in a layer of microfibers
    * eg velvet
    * https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_sheen
    */
    AiTextureType_SHEEN = 19,

    /** Clearcoat
    * Simulates a layer of 'polish' or 'laquer' layered on top of a PBR substrate
    * https://autodesk.github.io/standard-surface/#closures/coating
    * https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_clearcoat
    */
    AiTextureType_CLEARCOAT = 20,

    /** Transmission
    * Simulates transmission through the surface
    * May include further information such as wall thickness
    */
    AiTextureType_TRANSMISSION = 21,

    /** Unknown texture
     *
     *  A texture reference that does not match any of the definitions
     *  above is considered to be 'unknown'. It is still imported,
     *  but is excluded from any further post-processing.
    */
    AiTextureType_UNKNOWN = 18,

#ifndef SWIG
    _AiTextureType_Force32Bit = INT_MAX
#endif
};






// ---------------------------------------------------------------------------
/** @brief Defines all shading models supported by the library
 *
 *  Property: #AI_MATKEY_SHADING_MODEL
 *
 *  The list of shading modes has been taken from Blender.
 *  See Blender documentation for more information. The API does
 *  not distinguish between "specular" and "diffuse" shaders (thus the
 *  specular term for diffuse shading models like Oren-Nayar remains
 *  undefined). <br>
 *  Again, this value is just a hint. Assimp tries to select the shader whose
 *  most common implementation matches the original rendering results of the
 *  3D modeler which wrote a particular model as closely as possible.
 *
 */

UENUM(Blueprintable,BlueprintType)
enum EAiShadingMode {

 Null=0x0,
    /** Flat shading. Shading is done on per-face base,
     *  diffuse only. Also known as 'faceted shading'.
     */
    AiShadingMode_Flat = 0x1,

    /** Simple Gouraud shading.
     */
    AiShadingMode_Gouraud = 0x2,

    /** Phong-Shading -
     */
    AiShadingMode_Phong = 0x3,

    /** Phong-Blinn-Shading
     */
    AiShadingMode_Blinn = 0x4,

    /** Toon-Shading per pixel
     *
     *  Also known as 'comic' shader.
     */
    AiShadingMode_Toon = 0x5,

    /** OrenNayar-Shading per pixel
     *
     *  Extension to standard Lambertian shading, taking the
     *  roughness of the material into account
     */
    AiShadingMode_OrenNayar = 0x6,

    /** Minnaert-Shading per pixel
     *
     *  Extension to standard Lambertian shading, taking the
     *  "darkness" of the material into account
     */
    AiShadingMode_Minnaert = 0x7,

    /** CookTorrance-Shading per pixel
     *
     *  Special shader for metallic surfaces.
     */
    AiShadingMode_CookTorrance = 0x8,

    /** No shading at all. Constant light influence of 1.0.
    * Also known as "Unlit"
    */
    AiShadingMode_NoShading = 0x9,
    AiShadingMode_Unlit = aiShadingMode_NoShading, // Alias

    /** Fresnel shading
     */
    AiShadingMode_Fresnel = 0xa,

    /** Physically-Based Rendering (PBR) shading using
    * Bidirectional scattering/reflectance distribution function (BSDF/BRDF)
    * There are multiple methods under this banner, and model files may provide
    * data for more than one PBR-BRDF method.
    * Applications should use the set of provided properties to determine which
    * of their preferred PBR rendering methods are likely to be available
    * eg:
    * - If AI_MATKEY_METALLIC_FACTOR is set, then a Metallic/Roughness is available
    * - If AI_MATKEY_GLOSSINESS_FACTOR is set, then a Specular/Glossiness is available
    * Note that some PBR methods allow layering of techniques
    */
    AiShadingMode_PBR_BRDF = 0xb,

#ifndef SWIG
    _AiShadingMode_Force32Bit = INT_MAX
#endif
};

UCLASS()
class UE_ASSIMP_API UAIMaterial : public UObject
{
	GENERATED_BODY()

	friend  UAIScene;
	public:
	static UAIMaterial* InternalCreateNewObject(UObject* Parent,aiMaterial* InMaterial);

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Material")
	void GetMaterialBaseColor(FLinearColor& BaseColor) const;

	// -------------------------------------------------------------------
	/**
	* @brief  Returns the name of the material.
	* @return The name of the material.
	*/
	// -------------------------------------------------------------------
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Material")
	FString GetMaterialName() const;




 // ---------------------------------------------------------------------------
/** @brief Helper function to get all values pertaining to a particular
 *  texture slot from a material structure.
 *
 *  This function is provided just for convenience. You could also read the
 *  texture by parsing all of its properties manually. This function bundles
 *  all of them in a huge function monster.
 *
 *  @param[in] mat Pointer to the input material. May not be NULL
 *  @param[in] type Specifies the texture stack to read from (e.g. diffuse,
 *     specular, height map ...).
 *  @param[in] index Index of the texture. The function fails if the
 *     requested index is not available for this texture type.
 *     #aiGetMaterialTextureCount() can be used to determine the number of
 *     textures in a particular texture stack.
 *  @param[out] path Receives the output path
 *     If the texture is embedded, receives a '*' followed by the id of
 *     the texture (for the textures stored in the corresponding scene) which
 *     can be converted to an int using a function like atoi.
 *     This parameter must be non-null.
 *  @param mapping The texture mapping mode to be used.
 *      Pass NULL if you're not interested in this information.
 *  @param[out] uvindex For UV-mapped textures: receives the index of the UV
 *      source channel. Unmodified otherwise.
 *      Pass NULL if you're not interested in this information.
 *  @param[out] blend Receives the blend factor for the texture
 *      Pass NULL if you're not interested in this information.
 *  @param[out] op Receives the texture blend operation to be perform between
 *      this texture and the previous texture.
 *      Pass NULL if you're not interested in this information.
 *  @param[out] mapmode Receives the mapping modes to be used for the texture.
 *      Pass NULL if you're not interested in this information. Otherwise,
 *      pass a pointer to an array of two aiTextureMapMode's (one for each
 *      axis, UV order).
 *  @param[out] flags Receives the the texture flags.
 *  @return AI_SUCCESS on success, otherwise something else. Have fun.*/
// ---------------------------------------------------------------------------
 UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Material")
 EAssimpReturn GetMaterialTexture(
        EAiTextureType Type,
         uint8 Index,
        FString &Path,
        EAiTextureMapping Mapping 
       );
	
	private:
	aiMaterial* Material;
};
