// Copyright 2021 LIV Inc. - MIT License
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "LivEditorAnalytics.h"

class FToolBarBuilder;
class FMenuBuilder;

class FLivEditorModule : public IModuleInterface
{
public:

	static FLivEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FLivEditorModule>("LIVEditor");
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("LIVEditor");
	}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr<FLivEditorAnalytics> GetAnalytics() const;

private:

	TSharedPtr<class FUICommandList> PluginCommands;

	TSharedPtr<FLivEditorAnalytics> Analytics;
};
