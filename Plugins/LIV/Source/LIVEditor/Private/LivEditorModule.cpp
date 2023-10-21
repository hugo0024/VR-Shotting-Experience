// Copyright 2021 LIV Inc. - MIT License
#include "LivEditorModule.h"
#include "LivEditorStyle.h"
#include "LivDebuggingTab.h"
#include "Misc/MessageDialog.h"

#include "AssetRegistryModule.h"
#include "ISettingsCategory.h"
#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "LivEditorStyle.h"
#include "LivDebuggingTab.h"
#include "LivEditorSettings.h"
#include "MeshDescription.h"
#include "StaticMeshAttributes.h"
#include "ToolMenus.h"
#include "UnrealEd.h"
#include "Framework/Docking/WorkspaceItem.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"


static const FName LIVEditorTabName("LIVEditor");

#define LOCTEXT_NAMESPACE "FLIVEditorModule"

void FLivEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	Analytics = MakeShared<FLivEditorAnalytics>();

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		// register editor settings
		SettingsModule->RegisterSettings("Editor", "Plugins", "LIV",
			LOCTEXT("LivEditorSettingsName", "LIV Editor"),
			LOCTEXT("LivEditorSettingsDescription", "Configure LIV Editor Plugin Settings."),
			GetMutableDefault<ULivEditorSettings>()
		);

		// detect plugin settings changes for analytics
		if(TSharedPtr<ISettingsContainer> ProjectSettingsContainer = SettingsModule->GetContainer("Project"))
		{
			if(TSharedPtr<ISettingsCategory> PluginSettingsCategory = ProjectSettingsContainer->GetCategory("Plugins"))
			{
				if(TSharedPtr<ISettingsSection> LivSettingsSection = PluginSettingsCategory->GetSection("LIV"))
				{
					LivSettingsSection->OnModified().BindLambda([&]
					{
						Analytics->RecordEvent(TEXT("PLUGIN_SETTINGS_MODIFIED"));
						return true;
					});
				}
			}
		}
	}

	Analytics->RecordEvent(TEXT("EDITOR_STARTED"));
	
	FLivEditorStyle::Initialize();
	FLivEditorStyle::ReloadTextures();

	{
		const IWorkspaceMenuStructure& MenuStructure = WorkspaceMenu::GetMenuStructure();
		TSharedRef<FWorkspaceItem> MediaBrowserGroup = MenuStructure.GetDeveloperToolsMiscCategory()->GetParent()->AddGroup(
			LOCTEXT("WorkspaceMenu_MediaCategory", "Media"),
			FSlateIcon(),
			true);

		SLivDebuggingTab::RegisterNomadTabSpawner(MediaBrowserGroup);
	}
	
	PluginCommands = MakeShareable(new FUICommandList);
}

void FLivEditorModule::ShutdownModule()
{
	// unregister editor settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Editor", "Plugins", "LIV");
	}

	FLivEditorStyle::Shutdown();
}

TSharedPtr<FLivEditorAnalytics> FLivEditorModule::GetAnalytics() const
{
	return Analytics;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLivEditorModule, LIVEditor)