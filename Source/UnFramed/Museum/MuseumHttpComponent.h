#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HttpFwd.h"
#include "MuseumHttpComponent.generated.h"

class FJsonObject;

USTRUCT(BlueprintType)
struct FMuseumQuizResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Museum|HTTP")
	FString Description;

	UPROPERTY(BlueprintReadOnly, Category="Museum|HTTP")
	TArray<FString> QuizOptions;

	UPROPERTY(BlueprintReadOnly, Category="Museum|HTTP")
	int32 CorrectAnswerIndex = INDEX_NONE;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMuseumQuizRequestSucceeded, FMuseumQuizResponse, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMuseumQuizRequestFailed, FString, ErrorMessage);

UCLASS(ClassGroup=(Custom), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class UNFRAMED_API UMuseumHttpComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMuseumHttpComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Museum|HTTP")
	FString EndpointUrl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Museum|HTTP")
	FString ArtworkNameField = TEXT("artworkName");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Museum|HTTP")
	TMap<FString, FString> AdditionalHeaders;

	UPROPERTY(BlueprintReadOnly, Category="Museum|HTTP")
	bool bIsRequestInFlight = false;

	UPROPERTY(BlueprintReadOnly, Category="Museum|HTTP")
	FName LastRequestedArtworkName;

	UPROPERTY(BlueprintReadOnly, Category="Museum|HTTP")
	FMuseumQuizResponse LastResponse;

	UPROPERTY(BlueprintAssignable, Category="Museum|HTTP")
	FOnMuseumQuizRequestSucceeded OnQuizRequestSucceeded;

	UPROPERTY(BlueprintAssignable, Category="Museum|HTTP")
	FOnMuseumQuizRequestFailed OnQuizRequestFailed;

	UFUNCTION(BlueprintCallable, Category="Museum|HTTP")
	bool RequestArtworkQuiz(FName ArtworkName);

protected:
	virtual void BeginPlay() override;

private:
	void HandleRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	bool TryParseQuizResponse(const FString& ResponseBody, FMuseumQuizResponse& OutResponse, FString& OutErrorMessage) const;
	bool TryGetStringArrayField(const TSharedPtr<FJsonObject>& JsonObject, const TArray<FString>& CandidateFields, TArray<FString>& OutValues) const;
	bool TryGetStringField(const TSharedPtr<FJsonObject>& JsonObject, const TArray<FString>& CandidateFields, FString& OutValue) const;
	bool TryGetIntegerField(const TSharedPtr<FJsonObject>& JsonObject, const TArray<FString>& CandidateFields, int32& OutValue) const;
	void BroadcastFailure(const FString& ErrorMessage);
};
