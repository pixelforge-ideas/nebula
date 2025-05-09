#pragma once
//------------------------------------------------------------------------------
/**
    Container type for meshes

    @copyright
    (C) 2022 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "coregraphics/mesh.h"
namespace CoreGraphics
{

RESOURCE_ID_TYPE(MeshResourceId);
struct MeshResource
{
    Util::Array<CoreGraphics::MeshId> meshes;
};

/// Get mesh
const MeshId MeshResourceGetMesh(const MeshResourceId id, const IndexT index);
/// Get number of meshes
const SizeT MeshResourceGetNumMeshes(const MeshResourceId id);

/// Destroy
void DestroyMeshResource(const MeshResourceId id);


enum
{
    MeshResource_Meshes,
    MeshResource_VertexData,
    MeshResource_IndexData
};

typedef Ids::IdAllocator<
    Util::FixedArray<MeshId>,
    CoreGraphics::VertexAlloc,
    CoreGraphics::VertexAlloc
> MeshResourceAllocator;
extern MeshResourceAllocator meshResourceAllocator;

} // namespace CoreGraphics
