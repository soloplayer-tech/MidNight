#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Museum/MuseumHttpComponent.h"
#include "MuseumQuizUI.generated.h"

class AUnFramedCharacter;
class UButton;
class UMuseumHttpComponent;
class UTextBlock;
class UWidgetAnimation;

UCLASS(Abstract, BlueprintType, Blueprintable)
class UNFRAMED_API UMuseumQuizUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	void SetupWithCharacter(AUnFramedCharacter* Character);

	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	void SetupWithHttpComponent(UMuseumHttpComponent* InHttpComponent);

	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	bool RequestArtworkQuiz(FName ArtworkName);

	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	void ClearQuizState();

	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	bool SubmitAnswer(int32 AnswerIndex);

	UFUNCTION(BlueprintPure, Category="Museum|UI")
	bool HasValidQuiz() const;

	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	void SetWidgetVisibility(bool bVisible);

	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	void CloseQuizWidget();

	UPROPERTY(BlueprintReadOnly, Category="Museum|UI")
	FName CurrentArtworkName;

	UPROPERTY(BlueprintReadOnly, Category="Museum|UI")
	FString CurrentDescription;

	UPROPERTY(BlueprintReadOnly, Category="Museum|UI")
	TArray<FString> CurrentQuizOptions;

	UPROPERTY(BlueprintReadOnly, Category="Museum|UI")
	int32 CurrentCorrectAnswerIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category="Museum|UI")
	int32 SelectedAnswerIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category="Museum|UI")
	bool bIsWaitingForResponse = false;

	UPROPERTY(BlueprintReadOnly, Category="Museum|UI")
	bool bLastSubmittedAnswerWasCorrect = false;

	UPROPERTY(BlueprintReadOnly, Category="Museum|UI")
	FString LastErrorMessage;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleQuizRequestSucceeded(FMuseumQuizResponse Response);

	UFUNCTION()
	void HandleQuizRequestFailed(FString ErrorMessage);

private:
	void RefreshWidgetState();
	void UnbindFromHttpComponent();
	void BindAnswerButton(UButton* Button, int32 AnswerIndex);
	void BindCloseButton();
	void UpdateAnswerText(UTextBlock* TextBlock, int32 AnswerIndex) const;
	void UpdateAnswerButtonEnabledState(UButton* Button, int32 AnswerIndex) const;
	void UpdateAnswerButtonVisualState(UButton* Button, int32 AnswerIndex) const;
	void PlayWrongAnswerAnimation(int32 AnswerIndex);

	UFUNCTION()
	void HandleAnswerButton0Clicked();

	UFUNCTION()
	void HandleAnswerButton1Clicked();

	UFUNCTION()
	void HandleAnswerButton2Clicked();

	UFUNCTION()
	void HandleAnswerButton3Clicked();

	UFUNCTION()
	void HandleCloseButtonClicked();

	UPROPERTY(Transient)
	TObjectPtr<UMuseumHttpComponent> HttpComponent;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> ArtworkNameText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> DescriptionText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> StatusText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> AnswerText0;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> AnswerText1;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> AnswerText2;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> AnswerText3;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> AnswerButton0;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> AnswerButton1;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> AnswerButton2;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> AnswerButton3;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> CloseButton;

	UPROPERTY(EditDefaultsOnly, Category="Museum|UI")
	FLinearColor DefaultAnswerButtonColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, Category="Museum|UI")
	FLinearColor WrongAnswerButtonColor = FLinearColor(1.0f, 0.25f, 0.25f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category="Museum|UI")
	FLinearColor CorrectAnswerButtonColor = FLinearColor(0.35f, 1.0f, 0.45f, 1.0f);

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> WrongAnswerShakeAnimation0;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> WrongAnswerShakeAnimation1;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> WrongAnswerShakeAnimation2;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> WrongAnswerShakeAnimation3;
};
