// Copyright 2021 LIV Inc. - MIT License
#include "LivEditorAnalytics.h"
#include "HttpModule.h"
#include "LivEditorSettings.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/ScopeLock.h"
#include "Modules/ModuleManager.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonWriter.h"
#include "Runtime/Launch/Resources/Version.h"

static constexpr auto GFlushEventsCountdownDuration = 30.0f;

FLivEditorAnalytics::FLivEditorAnalytics()	
	: FlushEventsCountdown(GFlushEventsCountdownDuration)
{
	const FString ConfigFilePath = IPluginManager::Get().FindPlugin("LIV")->GetBaseDir() / TEXT("Config") / TEXT("LivAnalytics.ini");
	if(!GConfig->GetString(TEXT("Tracking"), TEXT("TrackingID"), TrackingID, ConfigFilePath))
	{
		TrackingID = FPlatformMisc::GetLoginId();
	}
}

FLivEditorAnalytics::~FLivEditorAnalytics()
{
	FlushEvents();
}

bool FLivEditorAnalytics::Tick(float DeltaTime)
{
	FScopeLock ScopedLock(&CachedEventsCS);

	if(!IsAnalyticsEnabled())
	{
		return true;
	}

	FlushEventsCountdown -= DeltaTime;
	// If reached countdown or already at max cached events then flush
	if (FlushEventsCountdown <= 0 && CachedEvents.Num() > 0)
	{
		FlushEvents();
	}
	
	return true;
}

void FLivEditorAnalytics::RecordEvent(const FString& EventName)
{
	RecordEvent(EventName, TArray<FAnalyticsEventAttribute>());
}

void FLivEditorAnalytics::RecordEvent(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attributes)
{
	// Have to copy Attributes array because this doesn't come in as an rvalue ref.
	RecordEvent(EventName, TArray<FAnalyticsEventAttribute>(Attributes));
}

void FLivEditorAnalytics::RecordEvent(FString EventName, TArray<FAnalyticsEventAttribute>&& Attributes)
{
	FScopeLock ScopedLock(&CachedEventsCS);
	if (IsAnalyticsEnabled())
	{
		CachedEvents.Emplace(MoveTemp(EventName), MoveTemp(Attributes), false, false);
	}
}

void FLivEditorAnalytics::OptOut()
{
	// special path to force the opt out analytic through
	CachedEvents.Emplace(TEXT("ANALYTICS_OPT_OUT"), TArray<FAnalyticsEventAttribute>(), false, false);
	FlushEvents();
}

bool FLivEditorAnalytics::IsAnalyticsEnabled() const
{
	return GetDefault<ULivEditorSettings>()->bAnalyticsEnabled;
}

void FLivEditorAnalytics::FlushEvents()
{
	FScopeLock ScopedLock(&CachedEventsCS);

	FHttpModule& HttpModule = FModuleManager::LoadModuleChecked<FHttpModule>("HTTP");

	FString Payload;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Payload);

	JsonWriter->WriteObjectStart();
	JsonWriter->WriteObjectStart(TEXT("client"));
	{
		JsonWriter->WriteValue(TEXT("name"), TEXT("liv-unreal-editor"));
		const FString VersionString = FString::Printf(TEXT("%s:%s"),
			ENGINE_VERSION_STRING, *IPluginManager::Get().FindPlugin(TEXT("Liv"))->GetDescriptor().VersionName);
		JsonWriter->WriteValue("version", *VersionString);
		JsonWriter->WriteValue(TEXT("uid"), *TrackingID);
	}
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart(TEXT("platform"));
	{
		const FString PlatformName = ANSI_TO_TCHAR(FPlatformProperties::IniPlatformName());
		JsonWriter->WriteValue(TEXT("os"), *PlatformName);
		JsonWriter->WriteValue(TEXT("version"), *FPlatformMisc::GetOSVersion());
		JsonWriter->WriteValue(TEXT("device"), *PlatformName);
	}
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteArrayStart(TEXT("events"));
	{
		JsonWriter->WriteObjectStart();

		for (auto& Entry : CachedEvents)
		{
			JsonWriter->WriteValue(TEXT("event"), Entry.EventName);

			if (Entry.Attributes.Num() > 0)
			{
				JsonWriter->WriteObjectStart(TEXT("properties"));
				for (const FAnalyticsEventAttribute& Attr : Entry.Attributes)
				{
					switch (Attr.AttrType)
					{
					case FAnalyticsEventAttribute::AttrTypeEnum::String:
						JsonWriter->WriteValue(Attr.AttrName, Attr.AttrValueString);
						break;
					case FAnalyticsEventAttribute::AttrTypeEnum::Number:
						JsonWriter->WriteValue(Attr.AttrName, Attr.ToString());
						break;
					case FAnalyticsEventAttribute::AttrTypeEnum::Boolean:
						JsonWriter->WriteValue(Attr.AttrName, Attr.ToString());
						break;
					case FAnalyticsEventAttribute::AttrTypeEnum::JsonFragment:
						JsonWriter->WriteRawJSONValue(Attr.AttrName, Attr.AttrValueString);
						break;
					}
				}
				JsonWriter->WriteObjectEnd();
			}
		}
		
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteObjectEnd();

	JsonWriter->Close();

	auto HttpRequest = HttpModule.CreateRequest();

	static const FString APIServer(TEXT("https://errors.liv.tv"));
	static const FString URLPath(TEXT("collect/mixpanel"));

	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json; charset=utf-8"));
	HttpRequest->SetURL(APIServer / URLPath);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetContentAsString(Payload);

	// we don't check if events went through currently
	HttpRequest->ProcessRequest();

	// clear events
	CachedEvents.Reset();

	// reset countdown
	FlushEventsCountdown = GFlushEventsCountdownDuration;
}
