#pragma once
//------------------------------------------------------------------------------
/**
    The ResourceServer marks the central entry point into the Resource subsystem.
    It contains a set of convenience functions (which is just a proxy for the Singleton),
    and should be updated at least once per frame using Update().
    
    @copyright
    (C) 2017-2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <functional>
#include "core/refcounted.h"
#include "ids/id.h"
#include "core/singleton.h"
#include "resourceid.h"
#include "resourceloader.h"
#include "resourceloaderthread.h"
namespace Resources
{
class ResourceServer : public Core::RefCounted
{
    __DeclareClass(ResourceServer);
    __DeclareInterfaceSingleton(ResourceServer);
public:
    /// constructor
    ResourceServer();
    /// destructor
    virtual ~ResourceServer();

    /// open manager
    void Open();
    /// close manager
    void Close();

    /// update resource manager, call each frame
    void Update(IndexT frameIndex);

    /// create a new resource (stream-managed), which will be loaded at some later point, if not already loaded
    Resources::ResourceId CreateResource(const ResourceName& res, std::function<void(const Resources::ResourceId)> success = nullptr, std::function<void(const Resources::ResourceId)> failed = nullptr, bool immediate = false, bool stream = true);
    /// overload which also takes an identifying tag, which is used to group-discard resources
    Resources::ResourceId CreateResource(const ResourceName& res, const Util::StringAtom& tag, std::function<void(const Resources::ResourceId)> success = nullptr, std::function<void(const Resources::ResourceId)> failed = nullptr, bool immediate = false, bool stream = true);
    /// create a new resource (stream-managed), which will be loaded at some later point, if not already loaded
    template<class METADATA> Resources::ResourceId CreateResource(const ResourceName& res, const METADATA& metaData, std::function<void(const Resources::ResourceId)> success = nullptr, std::function<void(const Resources::ResourceId)> failed = nullptr, bool immediate = false, bool stream = true);
    /// overload which also takes an identifying tag, which is used to group-discard resources
    template<class METADATA> Resources::ResourceId CreateResource(const ResourceName& res, const METADATA& metaData, const Util::StringAtom& tag, std::function<void(const Resources::ResourceId)> success = nullptr, std::function<void(const Resources::ResourceId)> failed = nullptr, bool immediate = false, bool stream = true);
    /// discard resource (stream-managed)
    void DiscardResource(const Resources::ResourceId res);
    /// discard all resources by tag (stream-managed)
    void DiscardResources(const Util::StringAtom& tag);
    /// returns true if there are pending resources in-flight
    bool HasPendingResources();
    /// reload resource
    void ReloadResource(const ResourceName& res, std::function<void(const Resources::ResourceId)> success = nullptr, std::function<void(const Resources::ResourceId)> failed = nullptr);
    /// stream in a new LOD
    void SetMinLod(const ResourceId& id, float lod, bool immediate);
    /// Create single-fire listener for resource. When resource is loaded, the callbacks will be invoked and the listener is destroyed
    void CreateResourceListener(const ResourceId& id, std::function<void(const Resources::ResourceId)> success, std::function<void(const Resources::ResourceId)> failed = nullptr);

    /// get type of resource pool this resource was allocated with
    Core::Rtti* GetType(const Resources::ResourceId id);

    /// get resource name
    const Resources::ResourceName GetName(const Resources::ResourceId id) const;
    /// get tag resource was first registered with
    const Util::StringAtom GetTag(const Resources::ResourceId id) const;
    /// get resource state
    const Resource::State GetState(const Resources::ResourceId id) const;
    /// get usage 
    const SizeT GetUsage(const Resources::ResourceId id) const;
    /// check if resource id is valid
    bool HasResource(const Resources::ResourceId id) const;
    /// get id from name
    const Resources::ResourceId GetId(const Resources::ResourceName& name) const;

    /// register a stream pool, which takes an extension and the RTTI of the resource type to create
    void RegisterStreamLoader(const Util::StringAtom& ext, const Core::Rtti& loaderClass);
    /// deregisters a stream pool
    void DeregisterStreamLoader(const Util::StringAtom& ext, const Core::Rtti& loaderClass);
    /// get stream pool for later use
    template <class POOL_TYPE> POOL_TYPE* GetStreamLoader() const;

    /// Wait for all loader threads
    void WaitForLoaderThread();

    /// goes through all pools and sets up their default resources
    void LoadDefaultResources();
private:
    friend class ResourceLoader;

    bool open;
    Util::Dictionary<Util::StringAtom, IndexT> extensionMap;
    Util::Dictionary<const Core::Rtti*, IndexT> typeMap;
    Util::Array<Ptr<ResourceLoader>> loaders;

    static int32_t UniquePoolCounter;
};

//------------------------------------------------------------------------------
/**
    If a previous call to CreateResources triggered a resource load, and this evocation enforces the resource loading to be immediate, then despite
    this call not actually triggering a resource to be loaded, the referenced resource will be loaded immediately nonetheless.
*/
inline Resources::ResourceId
Resources::ResourceServer::CreateResource(
    const ResourceName& id
    , std::function<void(const Resources::ResourceId)> success
    , std::function<void(const Resources::ResourceId)> failed
    , bool immediate
    , bool stream
)
{
    return this->CreateResource(id, "", success, failed, immediate, stream);
}

//------------------------------------------------------------------------------
/**
    If a previous call to CreateResources triggered a resource load, and this evocation enforces the resource loading to be immediate, then despite
    this call not actually triggering a resource to be loaded, the referenced resource will be loaded immediately nonetheless.
*/
inline Resources::ResourceId
Resources::ResourceServer::CreateResource(
    const ResourceName& res
    , const Util::StringAtom& tag
    , std::function<void(const Resources::ResourceId)> success
    , std::function<void(const Resources::ResourceId)> failed
    , bool immediate
    , bool stream
)
{
    // get resource loader by extension
    Util::String ext = res.AsString().GetFileExtension();
    IndexT i = this->extensionMap.FindIndex(ext);
    n_assert_fmt(i != InvalidIndex, "No resource loader is associated with file extension '%s'", ext.AsCharPtr());
    const Ptr<ResourceLoader>& loader = this->loaders[this->extensionMap.ValueAtIndex(i)].downcast<ResourceLoader>();

    // create container and cast to actual resource type
    Resources::ResourceId id = loader->CreateResource(res, nullptr, 0, tag, success, failed, immediate, stream);
    return id;
}

//------------------------------------------------------------------------------
/**
*/
template<class METADATA>
inline Resources::ResourceId
ResourceServer::CreateResource(
    const ResourceName& res
    , const METADATA& metaData
    , std::function<void(const Resources::ResourceId)> success
    , std::function<void(const Resources::ResourceId)> failed
    , bool immediate
    , bool stream
)
{
    return this->CreateResource(res, metaData, "", success, failed, immediate, stream);
}

//------------------------------------------------------------------------------
/**
*/
template<class METADATA>
inline Resources::ResourceId
ResourceServer::CreateResource(
    const ResourceName& res
    , const METADATA& metaData
    , const Util::StringAtom& tag
    , std::function<void(const Resources::ResourceId)> success
    , std::function<void(const Resources::ResourceId)> failed
    , bool immediate
    , bool stream
)
{
    // get resource loader by extension
    Util::String ext = res.AsString().GetFileExtension();
    IndexT i = this->extensionMap.FindIndex(ext);
    n_assert_fmt(i != InvalidIndex, "No resource loader is associated with file extension '%s'", ext.AsCharPtr());
    const Ptr<ResourceLoader>& loader = this->loaders[this->extensionMap.ValueAtIndex(i)].downcast<ResourceLoader>();

    // create container and cast to actual resource type
    Resources::ResourceId id = loader->CreateResource(res, &metaData, sizeof(METADATA), tag, success, failed, immediate, stream);
    return id;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceServer::ReloadResource(const ResourceName& res, std::function<void(const Resources::ResourceId)> success, std::function<void(const Resources::ResourceId)> failed)
{
    // get resource loader by extension
    Util::String ext = res.AsString().GetFileExtension();
    IndexT i = this->extensionMap.FindIndex(ext);
    n_assert_fmt(i != InvalidIndex, "No resource loader is associated with file extension '%s'", ext.AsCharPtr());
    const Ptr<ResourceLoader>& loader = this->loaders[this->extensionMap.ValueAtIndex(i)].downcast<ResourceLoader>();

    // create container and cast to actual resource type
    loader->ReloadResource(res, success, failed);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ResourceServer::SetMinLod(const ResourceId& id, float lod, bool immediate)
{
    // get id of loader
    const Ids::Id8 loaderid = id.loaderIndex;

    // get resource loader by extension
    n_assert(this->loaders.Size() > loaderid);
    const Ptr<ResourceLoader>& loader = this->loaders[loaderid].downcast<ResourceLoader>();

    // update LOD
    loader->SetMinLod(id, lod, immediate);
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceServer::CreateResourceListener(
    const ResourceId& id,
    std::function<void(const Resources::ResourceId)> success,
    std::function<void(const Resources::ResourceId)> failed
)
{
    // get id of loader
    const Ids::Id8 loaderid = id.loaderIndex;

    // get resource loader by extension
    n_assert(this->loaders.Size() > loaderid);
    const Ptr<ResourceLoader>& loader = this->loaders[loaderid].downcast<ResourceLoader>();

    loader->CreateListener(id, success, failed);
}

//------------------------------------------------------------------------------
/**
    Discards a single resource, and removes the callbacks to it from
*/
inline void
ResourceServer::DiscardResource(const Resources::ResourceId id)
{
    // get id of loader
    const Ids::Id8 loaderid = id.loaderIndex;

    // get resource loader by extension
    n_assert(this->loaders.Size() > loaderid);
    const Ptr<ResourceLoader>& loader = this->loaders[loaderid].downcast<ResourceLoader>();

    // discard container
    loader->DiscardResource(id);
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceName
ResourceServer::GetName(const Resources::ResourceId id) const
{
    // get resource loader by extension
    n_assert(this->loaders.Size() > id.loaderIndex);
    const Ptr<ResourceLoader>& loader = this->loaders[id.loaderIndex];
    return loader->GetName(id.resourceId);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom
ResourceServer::GetTag(const Resources::ResourceId id) const
{
    // get resource loader by extension
    n_assert(this->loaders.Size() > id.loaderIndex);
    const Ptr<ResourceLoader>& loader = this->loaders[id.loaderIndex];
    return loader->GetTag(id.resourceId);
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::Resource::State
ResourceServer::GetState(const Resources::ResourceId id) const
{
    // get resource loader by extension
    n_assert(this->loaders.Size() > id.loaderIndex);
    const Ptr<ResourceLoader>& loader = this->loaders[id.loaderIndex];
    return loader->GetState(id.resourceId);
}

//------------------------------------------------------------------------------
/**
*/
inline const SizeT
ResourceServer::GetUsage(const Resources::ResourceId id) const
{
    // get resource loader by extension
    n_assert(this->loaders.Size() > id.loaderIndex);
    const Ptr<ResourceLoader>& loader = this->loaders[id.loaderIndex];
    return loader->GetUsage(id.resourceId);
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ResourceServer::HasResource(const Resources::ResourceId id) const
{
    if (this->loaders.Size() <= id.loaderIndex) return false;
    {
        const Ptr<ResourceLoader>& loader = this->loaders[id.loaderIndex];
        if (loader->HasResource(id)) return true;
        return false;		
    }
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId
ResourceServer::GetId(const Resources::ResourceName& name) const
{
    IndexT i;
    for (i = 0; i < this->loaders.Size(); i++)
    {
        Resources::ResourceId id = this->loaders[i]->GetId(name);
        if (id != Resources::ResourceId::Invalid()) return id;
    }
    return Resources::ResourceId::Invalid();
}

//------------------------------------------------------------------------------
/**
*/
template <class POOL_TYPE>
inline POOL_TYPE*
ResourceServer::GetStreamLoader() const
{
    static_assert(std::is_base_of<ResourceLoader, POOL_TYPE>::value, "Type requested is not a stream pool");
    IndexT i = this->typeMap.FindIndex(&POOL_TYPE::RTTI);
    if (i != InvalidIndex)
    {
        return static_cast<POOL_TYPE*>(this->loaders[this->typeMap.ValueAtIndex(i)].get());
    }

    n_error("No loader registered for this type");
    return nullptr;
}

//------------------------------------------------------------------------------
/**
*/
inline Resources::ResourceId
CreateResource(
    const ResourceName& res
    , const Util::StringAtom& tag
    , std::function<void(const Resources::ResourceId)> success = nullptr
    , std::function<void(const Resources::ResourceId)> failed = nullptr
    , bool immediate = false
    , bool stream = true
)
{
    return ResourceServer::Instance()->CreateResource(res, tag, success, failed, immediate, stream);
}

//------------------------------------------------------------------------------
/**
*/
template <class METADATA>
inline Resources::ResourceId
CreateResource(
    const ResourceName& res
    , const METADATA& metaData
    , const Util::StringAtom& tag
    , std::function<void(const Resources::ResourceId)> success = nullptr
    , std::function<void(const Resources::ResourceId)> failed = nullptr
    , bool immediate = false
    , bool stream = true
)
{
    return ResourceServer::Instance()->CreateResource(res, metaData, tag, success, failed, immediate, stream);
}

//------------------------------------------------------------------------------
/**
*/
inline void
CreateResourceListener(
    const ResourceId& id
    , std::function<void(const Resources::ResourceId)> success
    , std::function<void(const Resources::ResourceId)> failed = nullptr
)
{
    return ResourceServer::Instance()->CreateResourceListener(id, success, failed);
}

//------------------------------------------------------------------------------
/**
*/
inline void
SetMinLod(const ResourceId& id, float lod, bool immediate)
{
    return ResourceServer::Instance()->SetMinLod(id, lod, immediate);
}

//------------------------------------------------------------------------------
/**
*/
inline void
DiscardResource(const Resources::ResourceId id)
{
    ResourceServer::Instance()->DiscardResource(id);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ReloadResource(const ResourceName& res)
{
    return ResourceServer::Instance()->ReloadResource(res);
}

//------------------------------------------------------------------------------
/**
*/
inline void
WaitForLoaderThread()
{
    ResourceServer::Instance()->WaitForLoaderThread();
}

//------------------------------------------------------------------------------
/**
*/
template <class POOL_TYPE>
inline POOL_TYPE*
GetStreamLoader()
{
    static_assert(std::is_base_of<ResourceLoader, POOL_TYPE>::value, "Template argument is not a ResourceCache type!");
    return ResourceServer::Instance()->GetStreamLoader<POOL_TYPE>();
}

} // namespace Resources
