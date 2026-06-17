#include "Museum/MuseumHttpComponent.h"

#include "Dom/JsonObject.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

UMuseumHttpComponent::UMuseumHttpComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMuseumHttpComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UMuseumHttpComponent::RequestArtworkQuiz(FName ArtworkName)
{
	if (bIsRequestInFlight)
	{
		BroadcastFailure(TEXT("A quiz request is already in flight."));
		return false;
	}

	if (EndpointUrl.IsEmpty())
	{
		BroadcastFailure(TEXT("EndpointUrl is empty."));
		return false;
	}

	if (ArtworkName.IsNone())
	{
		BroadcastFailure(TEXT("ArtworkName is None."));
		return false;
	}

	TSharedRef<FJsonObject> RequestObject = MakeShared<FJsonObject>();
	RequestObject->SetStringField(ArtworkNameField, ArtworkName.ToString());

	FString RequestBody;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	if (!FJsonSerializer::Serialize(RequestObject, Writer))
	{
		BroadcastFailure(TEXT("Failed to serialize request body."));
		return false;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(EndpointUrl);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));

	for (const TPair<FString, FString>& HeaderPair : AdditionalHeaders)
	{
		HttpRequest->SetHeader(HeaderPair.Key, HeaderPair.Value);
	}

	HttpRequest->SetContentAsString(RequestBody);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UMuseumHttpComponent::HandleRequestCompleted);

	bIsRequestInFlight = true;
	LastRequestedArtworkName = ArtworkName;

	if (!HttpRequest->ProcessRequest())
	{
		bIsRequestInFlight = false;
		BroadcastFailure(TEXT("Failed to start HTTP request."));
		return false;
	}

	return true;
}

bool UMuseumHttpComponent::RequestArtworkExplanation(FName ArtworkName)
{
	if (bIsRequestInFlight)
	{
		BroadcastExplainFailure(TEXT("Another HTTP request is already in flight."));
		return false;
	}

	if (ExplainEndpointUrl.IsEmpty())
	{
		BroadcastExplainFailure(TEXT("ExplainEndpointUrl is empty."));
		return false;
	}

	if (ArtworkName.IsNone())
	{
		BroadcastExplainFailure(TEXT("ArtworkName is None."));
		return false;
	}

	TSharedRef<FJsonObject> RequestObject = MakeShared<FJsonObject>();
	RequestObject->SetStringField(ArtworkNameField, ArtworkName.ToString());

	FString RequestBody;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	if (!FJsonSerializer::Serialize(RequestObject, Writer))
	{
		BroadcastExplainFailure(TEXT("Failed to serialize explanation request body."));
		return false;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(ExplainEndpointUrl);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));

	for (const TPair<FString, FString>& HeaderPair : AdditionalHeaders)
	{
		HttpRequest->SetHeader(HeaderPair.Key, HeaderPair.Value);
	}

	HttpRequest->SetContentAsString(RequestBody);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UMuseumHttpComponent::HandleExplainRequestCompleted);

	bIsRequestInFlight = true;
	LastRequestedArtworkName = ArtworkName;

	if (!HttpRequest->ProcessRequest())
	{
		bIsRequestInFlight = false;
		BroadcastExplainFailure(TEXT("Failed to start explanation HTTP request."));
		return false;
	}

	return true;
}

void UMuseumHttpComponent::HandleRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	bIsRequestInFlight = false;

	if (!bWasSuccessful || !Response.IsValid())
	{
		BroadcastFailure(TEXT("HTTP request failed or returned no response."));
		return;
	}

	if (!EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		BroadcastFailure(FString::Printf(TEXT("HTTP request failed with status code %d."), Response->GetResponseCode()));
		return;
	}

	FString ErrorMessage;
	FMuseumQuizResponse ParsedResponse;
	if (!TryParseQuizResponse(Response->GetContentAsString(), ParsedResponse, ErrorMessage))
	{
		BroadcastFailure(ErrorMessage);
		return;
	}

	LastResponse = ParsedResponse;
	OnQuizRequestSucceeded.Broadcast(LastResponse);
}

void UMuseumHttpComponent::HandleExplainRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	bIsRequestInFlight = false;

	if (!bWasSuccessful || !Response.IsValid())
	{
		BroadcastExplainFailure(TEXT("HTTP explanation request failed or returned no response."));
		return;
	}

	if (!EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		BroadcastExplainFailure(FString::Printf(TEXT("HTTP explanation request failed with status code %d."), Response->GetResponseCode()));
		return;
	}

	FString ErrorMessage;
	FString ExplanationText;
	if (!TryParseExplainResponse(Response->GetContentAsString(), ExplanationText, ErrorMessage))
	{
		BroadcastExplainFailure(ErrorMessage);
		return;
	}

	LastExplanationText = ExplanationText;
	OnExplainRequestSucceeded.Broadcast(LastRequestedArtworkName, LastExplanationText);
}

bool UMuseumHttpComponent::TryParseQuizResponse(const FString& ResponseBody, FMuseumQuizResponse& OutResponse, FString& OutErrorMessage) const
{
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		OutErrorMessage = TEXT("Failed to parse response JSON.");
		return false;
	}

	if (!TryGetStringField(JsonObject, {TEXT("description"), TEXT("Description"), TEXT("명화설명")}, OutResponse.Description))
	{
		OutErrorMessage = TEXT("Response JSON is missing description.");
		return false;
	}

	if (!TryGetStringArrayField(JsonObject, {TEXT("quizOptions"), TEXT("QuizOptions"), TEXT("quiz"), TEXT("명화퀴즈")}, OutResponse.QuizOptions))
	{
		OutErrorMessage = TEXT("Response JSON is missing quiz options.");
		return false;
	}

	if (!TryGetIntegerField(JsonObject, {TEXT("correctAnswerIndex"), TEXT("CorrectAnswerIndex"), TEXT("answerIndex"), TEXT("정답번호")}, OutResponse.CorrectAnswerIndex))
	{
		OutErrorMessage = TEXT("Response JSON is missing correct answer index.");
		return false;
	}

	return true;
}

bool UMuseumHttpComponent::TryParseExplainResponse(const FString& ResponseBody, FString& OutExplanationText, FString& OutErrorMessage) const
{
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		if (TryGetStringField(JsonObject, {TEXT("description"), TEXT("Description"), TEXT("explanation"), TEXT("Explanation"), TEXT("content"), TEXT("Content"), TEXT("text"), TEXT("명화설명")}, OutExplanationText))
		{
			return true;
		}

		OutErrorMessage = TEXT("Explanation response JSON is missing explanation text.");
		return false;
	}

	OutExplanationText = ResponseBody;
	OutExplanationText.TrimStartAndEndInline();
	if (!OutExplanationText.IsEmpty())
	{
		return true;
	}

	OutErrorMessage = TEXT("Explanation response was empty.");
	return false;
}

bool UMuseumHttpComponent::TryGetStringArrayField(const TSharedPtr<FJsonObject>& JsonObject, const TArray<FString>& CandidateFields, TArray<FString>& OutValues) const
{
	for (const FString& FieldName : CandidateFields)
	{
		const TArray<TSharedPtr<FJsonValue>>* JsonValues = nullptr;
		if (!JsonObject->TryGetArrayField(FieldName, JsonValues) || JsonValues == nullptr)
		{
			continue;
		}

		OutValues.Reset();
		for (const TSharedPtr<FJsonValue>& JsonValue : *JsonValues)
		{
			FString StringValue;
			if (!JsonValue.IsValid() || !JsonValue->TryGetString(StringValue))
			{
				return false;
			}

			OutValues.Add(StringValue);
		}

		return true;
	}

	return false;
}

bool UMuseumHttpComponent::TryGetStringField(const TSharedPtr<FJsonObject>& JsonObject, const TArray<FString>& CandidateFields, FString& OutValue) const
{
	for (const FString& FieldName : CandidateFields)
	{
		if (JsonObject->TryGetStringField(FieldName, OutValue))
		{
			return true;
		}
	}

	return false;
}

bool UMuseumHttpComponent::TryGetIntegerField(const TSharedPtr<FJsonObject>& JsonObject, const TArray<FString>& CandidateFields, int32& OutValue) const
{
	for (const FString& FieldName : CandidateFields)
	{
		double NumericValue = 0.0;
		if (JsonObject->TryGetNumberField(FieldName, NumericValue))
		{
			OutValue = static_cast<int32>(NumericValue);
			return true;
		}
	}

	return false;
}

void UMuseumHttpComponent::BroadcastFailure(const FString& ErrorMessage)
{
	OnQuizRequestFailed.Broadcast(ErrorMessage);
}

void UMuseumHttpComponent::BroadcastExplainFailure(const FString& ErrorMessage)
{
	OnExplainRequestFailed.Broadcast(ErrorMessage);
}
