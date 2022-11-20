/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonScriptInterface.cpp
-------------------------------------------
*/

#include "IonScriptInterface.h"

#include "resources/IonFileResourceManager.h"

namespace ion::script::interfaces
{

using namespace script_interface;

namespace script_interface::detail
{

} //script_interface::detail


//Script interface
//Protected

bool ScriptInterface::Load(std::string_view asset_name)
{
	//Build from script (if found in script repository)
	if (auto repository = detail::repository_from_resource_name(ScriptRepositories(), asset_name); repository)
	{
		builder_.Compiler().BuildRepository(*repository);
		builder_.Validator(GetValidator());
		
		//Built and validated successfully
		if (builder_.Build(asset_name))
		{
			tree_ = builder_.Tree();
			return tree_.has_value();
		}
	}

	//Deserialize from object file (if found in file repository)
	if (auto data = detail::file_data_from_resource_name(FileRepositories(), asset_name); data)
	{
		tree_ = ion::script::ScriptTree::Deserialize(*data);
		//Should already be validated
		return tree_.has_value();
	}
	else
	{
		tree_ = {};
		return false;
	}
}


//Protected


//Public

/*
	Script repositories - Creating
*/

NonOwningPtr<assets::repositories::ScriptRepository> ScriptInterface::CreateScriptRepository()
{
	return ScriptRepositoryBase::Create();
}

NonOwningPtr<assets::repositories::ScriptRepository> ScriptInterface::CreateScriptRepository(const assets::repositories::ScriptRepository &repository)
{
	return ScriptRepositoryBase::Create(repository);
}

NonOwningPtr<assets::repositories::ScriptRepository> ScriptInterface::CreateScriptRepository(assets::repositories::ScriptRepository &&repository)
{
	return ScriptRepositoryBase::Create(std::move(repository));
}


/*
	Script repositories - Removing
*/

void ScriptInterface::ClearScriptRepositories() noexcept
{
	ScriptRepositoryBase::Clear();
}

bool ScriptInterface::RemoveScriptRepository(assets::repositories::ScriptRepository &repository) noexcept
{
	return ScriptRepositoryBase::Remove(repository);
}


/*
	File repositories - Creating
*/

NonOwningPtr<assets::repositories::FileRepository> ScriptInterface::CreateFileRepository()
{
	return FileRepositoryBase::Create();
}

NonOwningPtr<assets::repositories::FileRepository> ScriptInterface::CreateFileRepository(const assets::repositories::FileRepository &repository)
{
	return FileRepositoryBase::Create(repository);
}

NonOwningPtr<assets::repositories::FileRepository> ScriptInterface::CreateFileRepository(assets::repositories::FileRepository &&repository)
{
	return FileRepositoryBase::Create(std::move(repository));
}


/*
	File repositories - Removing
*/

void ScriptInterface::ClearFileRepositories() noexcept
{
	FileRepositoryBase::Clear();
}

bool ScriptInterface::RemoveFileRepository(assets::repositories::FileRepository &repository) noexcept
{
	return FileRepositoryBase::Remove(repository);
}

} //ion::script::interfaces