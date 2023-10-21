// Copyright 2021 LIV Inc. - MIT License
#include "HttpModule.h"
#include "HttpRetrySystem.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonWriter.h"
#include "Interfaces/IHttpResponse.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "LIV.h"
#include "Windows/HideWindowsPlatformTypes.h"

class FWaitForHttpRequestLatentCommand : public IAutomationLatentCommand \
{
public:
	FWaitForHttpRequestLatentCommand(TSharedRef<IHttpRequest, ESPMode::ThreadSafe> InRequest, double InTimeout)
		: Request(InRequest)
		, Timeout(InTimeout)
	{
		Request->ProcessRequest();
	}
	virtual ~FWaitForHttpRequestLatentCommand() {}
	virtual bool Update() override
	{
		const double NewTime = FPlatformTime::Seconds();

		if (NewTime - StartTime >= Timeout)
		{
			return true;
		}

		switch(Request->GetStatus())
		{
		case EHttpRequestStatus::Succeeded:
		case EHttpRequestStatus::Failed:
		case EHttpRequestStatus::Failed_ConnectionError:
			return true;
		default:
			return false;
		}
	}
private:
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request;
	double Timeout;
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLivTelemetryTest, "Liv.Editor.Telemetry Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLivTelemetryTest::RunTest(const FString& Parameters)
{
	FHttpModule& HttpModule = FModuleManager::LoadModuleChecked<FHttpModule>("HTTP");

	FString Payload;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Payload);

	JsonWriter->WriteObjectStart();
	JsonWriter->WriteObjectStart(TEXT("client"));
	{
		JsonWriter->WriteValue(TEXT("name"), TEXT("liv-unreal-editor"));
		JsonWriter->WriteValue("version", IPluginManager::Get()
			.FindPlugin(TEXT("Liv"))->GetDescriptor().VersionName);
		JsonWriter->WriteValue(TEXT("uid"), *FPlatformMisc::GetLoginId());
		/*JsonWriter->WriteValue(TEXT("uid"), FString::Printf(TEXT("%s|%s|%s"), 
			*FPlatformMisc::GetLoginId(), 
			*FPlatformMisc::GetEpicAccountId(), 
			*FPlatformMisc::GetOperatingSystemId()));*/
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
		{
			JsonWriter->WriteValue(TEXT("event"), TEXT("Test event"));
		}
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteObjectEnd();

	JsonWriter->Close();

	auto HttpRequest = HttpModule.CreateRequest();

	const FString APIServer(TEXT("https://errors.liv.tv"));
	const FString URLPath(TEXT("collect/mixpanel"));
	
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json; charset=utf-8"));
	HttpRequest->SetURL(APIServer / URLPath);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetContentAsString(Payload);

	AddInfo(FString::Printf(TEXT("Request Content: %s"), *Payload));

	HttpRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bConnectedSuccessfully)
	{
		TestEqual(TEXT("Response Code is 200"), ResponsePtr->GetResponseCode(), 200);
		AddInfo(FString::Printf(TEXT("Response: %s"), *ResponsePtr->GetContentAsString()));
		AddInfo(FString::Printf(TEXT("Response Code: %d"), ResponsePtr->GetResponseCode()));
	});

	ADD_LATENT_AUTOMATION_COMMAND(FWaitForHttpRequestLatentCommand(HttpRequest, 5.0));
	
	return true;
}


