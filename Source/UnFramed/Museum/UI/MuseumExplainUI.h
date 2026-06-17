#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MuseumExplainUI.generated.h"

class UButton;
class UTextBlock;

UCLASS(Abstract, BlueprintType, Blueprintable)
class UNFRAMED_API UMuseumExplainUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	void SetArtworkName(FName InArtworkName);

	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	void SetExplanationText(const FString& InExplanationText);

	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	void SetErrorText(const FString& InErrorText);

	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	void SetWidgetVisibility(bool bVisible);

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void HandleCloseButtonClicked();

	void RefreshWidgetState();

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> ArtworkNameText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> ExplanationTextBlock;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> ErrorTextBlock;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> CloseButton;

	FName ArtworkName;
	FString ExplanationText;
	FString ErrorText;
};
