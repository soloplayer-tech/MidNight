#include "Museum/UI/MuseumQuizUI.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UnFramedCharacter.h"
#include "Animation/WidgetAnimation.h"

void UMuseumQuizUI::SetupWithCharacter(AUnFramedCharacter* Character)
{
	SetupWithHttpComponent(Character ? Character->GetMuseumHttpComponent() : nullptr);
}

void UMuseumQuizUI::SetupWithHttpComponent(UMuseumHttpComponent* InHttpComponent)
{
	if (HttpComponent == InHttpComponent)
	{
		return;
	}

	UnbindFromHttpComponent();
	HttpComponent = InHttpComponent;

	if (HttpComponent)
	{
		HttpComponent->OnQuizRequestSucceeded.AddDynamic(this, &UMuseumQuizUI::HandleQuizRequestSucceeded);
		HttpComponent->OnQuizRequestFailed.AddDynamic(this, &UMuseumQuizUI::HandleQuizRequestFailed);
	}
}

bool UMuseumQuizUI::RequestArtworkQuiz(FName ArtworkName)
{
	if (!HttpComponent)
	{
		LastErrorMessage = TEXT("HttpComponent is not set.");
		RefreshWidgetState();
		return false;
	}

	ClearQuizState();

	if (!HttpComponent->RequestArtworkQuiz(ArtworkName))
	{
		LastErrorMessage = TEXT("Failed to start quiz request.");
		RefreshWidgetState();
		return false;
	}

	bIsWaitingForResponse = true;
	CurrentArtworkName = ArtworkName;
	RefreshWidgetState();
	return true;
}

void UMuseumQuizUI::ClearQuizState()
{
	CurrentArtworkName = NAME_None;
	CurrentDescription.Reset();
	CurrentQuizOptions.Reset();
	CurrentCorrectAnswerIndex = INDEX_NONE;
	SelectedAnswerIndex = INDEX_NONE;
	bIsWaitingForResponse = false;
	bLastSubmittedAnswerWasCorrect = false;
	LastErrorMessage.Reset();
	RefreshWidgetState();
}

bool UMuseumQuizUI::SubmitAnswer(int32 AnswerIndex)
{
	if (!HasValidQuiz() || !CurrentQuizOptions.IsValidIndex(AnswerIndex))
	{
		return false;
	}

	SelectedAnswerIndex = AnswerIndex;
	bLastSubmittedAnswerWasCorrect = (AnswerIndex == CurrentCorrectAnswerIndex);
	RefreshWidgetState();

	if (!bLastSubmittedAnswerWasCorrect)
	{
		PlayWrongAnswerAnimation(AnswerIndex);
	}

	return bLastSubmittedAnswerWasCorrect;
}

bool UMuseumQuizUI::HasValidQuiz() const
{
	return !CurrentDescription.IsEmpty()
		&& CurrentQuizOptions.Num() > 0
		&& CurrentQuizOptions.IsValidIndex(CurrentCorrectAnswerIndex);
}

void UMuseumQuizUI::SetWidgetVisibility(bool bVisible)
{
	SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UMuseumQuizUI::CloseQuizWidget()
{
	SetWidgetVisibility(false);
}

void UMuseumQuizUI::NativeConstruct()
{
	Super::NativeConstruct();

	BindAnswerButton(AnswerButton0, 0);
	BindAnswerButton(AnswerButton1, 1);
	BindAnswerButton(AnswerButton2, 2);
	BindAnswerButton(AnswerButton3, 3);
	BindCloseButton();

	RefreshWidgetState();
}

void UMuseumQuizUI::NativeDestruct()
{
	UnbindFromHttpComponent();
	Super::NativeDestruct();
}

void UMuseumQuizUI::HandleQuizRequestSucceeded(FMuseumQuizResponse Response)
{
	bIsWaitingForResponse = false;
	LastErrorMessage.Reset();
	CurrentDescription = Response.Description;
	CurrentQuizOptions = Response.QuizOptions;
	CurrentCorrectAnswerIndex = Response.CorrectAnswerIndex;
	SelectedAnswerIndex = INDEX_NONE;
	bLastSubmittedAnswerWasCorrect = false;
	RefreshWidgetState();
}

void UMuseumQuizUI::HandleQuizRequestFailed(FString ErrorMessage)
{
	bIsWaitingForResponse = false;
	LastErrorMessage = ErrorMessage;
	RefreshWidgetState();
}

void UMuseumQuizUI::RefreshWidgetState()
{
	if (ArtworkNameText)
	{
		ArtworkNameText->SetText(FText::FromName(CurrentArtworkName));
	}

	if (DescriptionText)
	{
		DescriptionText->SetText(FText::FromString(CurrentDescription));
	}

	if (StatusText)
	{
		FString StatusString;
		if (bIsWaitingForResponse)
		{
			StatusString = TEXT("Loading...");
		}
		else if (!LastErrorMessage.IsEmpty())
		{
			StatusString = LastErrorMessage;
		}
		else if (SelectedAnswerIndex != INDEX_NONE)
		{
			StatusString = bLastSubmittedAnswerWasCorrect ? TEXT("Correct") : TEXT("Incorrect");
		}
		else if (HasValidQuiz())
		{
			StatusString = TEXT("Choose an answer.");
		}
		else
		{
			StatusString = TEXT("No quiz loaded.");
		}

		StatusText->SetText(FText::FromString(StatusString));
	}

	UpdateAnswerText(AnswerText0, 0);
	UpdateAnswerText(AnswerText1, 1);
	UpdateAnswerText(AnswerText2, 2);
	UpdateAnswerText(AnswerText3, 3);

	UpdateAnswerButtonEnabledState(AnswerButton0, 0);
	UpdateAnswerButtonEnabledState(AnswerButton1, 1);
	UpdateAnswerButtonEnabledState(AnswerButton2, 2);
	UpdateAnswerButtonEnabledState(AnswerButton3, 3);

	UpdateAnswerButtonVisualState(AnswerButton0, 0);
	UpdateAnswerButtonVisualState(AnswerButton1, 1);
	UpdateAnswerButtonVisualState(AnswerButton2, 2);
	UpdateAnswerButtonVisualState(AnswerButton3, 3);

	if (CloseButton)
	{
		const bool bShowCloseButton = HasValidQuiz() && SelectedAnswerIndex != INDEX_NONE && bLastSubmittedAnswerWasCorrect;
		CloseButton->SetVisibility(bShowCloseButton ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UMuseumQuizUI::UnbindFromHttpComponent()
{
	if (HttpComponent)
	{
		HttpComponent->OnQuizRequestSucceeded.RemoveDynamic(this, &UMuseumQuizUI::HandleQuizRequestSucceeded);
		HttpComponent->OnQuizRequestFailed.RemoveDynamic(this, &UMuseumQuizUI::HandleQuizRequestFailed);
		HttpComponent = nullptr;
	}
}

void UMuseumQuizUI::BindAnswerButton(UButton* Button, int32 AnswerIndex)
{
	if (!Button)
	{
		return;
	}

	Button->OnClicked.Clear();

	switch (AnswerIndex)
	{
	case 0:
		Button->OnClicked.AddDynamic(this, &UMuseumQuizUI::HandleAnswerButton0Clicked);
		break;
	case 1:
		Button->OnClicked.AddDynamic(this, &UMuseumQuizUI::HandleAnswerButton1Clicked);
		break;
	case 2:
		Button->OnClicked.AddDynamic(this, &UMuseumQuizUI::HandleAnswerButton2Clicked);
		break;
	case 3:
		Button->OnClicked.AddDynamic(this, &UMuseumQuizUI::HandleAnswerButton3Clicked);
		break;
	default:
		break;
	}
}

void UMuseumQuizUI::BindCloseButton()
{
	if (!CloseButton)
	{
		return;
	}

	CloseButton->OnClicked.Clear();
	CloseButton->OnClicked.AddDynamic(this, &UMuseumQuizUI::HandleCloseButtonClicked);
}

void UMuseumQuizUI::UpdateAnswerText(UTextBlock* TextBlock, int32 AnswerIndex) const
{
	if (!TextBlock)
	{
		return;
	}

	if (CurrentQuizOptions.IsValidIndex(AnswerIndex))
	{
		TextBlock->SetText(FText::FromString(CurrentQuizOptions[AnswerIndex]));
	}
	else
	{
		TextBlock->SetText(FText::GetEmpty());
	}
}

void UMuseumQuizUI::UpdateAnswerButtonEnabledState(UButton* Button, int32 AnswerIndex) const
{
	if (!Button)
	{
		return;
	}

	bool bButtonEnabled = !bIsWaitingForResponse && CurrentQuizOptions.IsValidIndex(AnswerIndex);

	if (SelectedAnswerIndex != INDEX_NONE && bLastSubmittedAnswerWasCorrect && AnswerIndex != SelectedAnswerIndex)
	{
		bButtonEnabled = false;
	}

	Button->SetIsEnabled(bButtonEnabled);
}

void UMuseumQuizUI::UpdateAnswerButtonVisualState(UButton* Button, int32 AnswerIndex) const
{
	if (!Button)
	{
		return;
	}

	FLinearColor ButtonColor = DefaultAnswerButtonColor;

	if (SelectedAnswerIndex == AnswerIndex)
	{
		ButtonColor = bLastSubmittedAnswerWasCorrect ? CorrectAnswerButtonColor : WrongAnswerButtonColor;
	}

	Button->SetBackgroundColor(ButtonColor);
}

void UMuseumQuizUI::PlayWrongAnswerAnimation(int32 AnswerIndex)
{
	UWidgetAnimation* AnimationToPlay = nullptr;

	switch (AnswerIndex)
	{
	case 0:
		AnimationToPlay = WrongAnswerShakeAnimation0;
		break;
	case 1:
		AnimationToPlay = WrongAnswerShakeAnimation1;
		break;
	case 2:
		AnimationToPlay = WrongAnswerShakeAnimation2;
		break;
	case 3:
		AnimationToPlay = WrongAnswerShakeAnimation3;
		break;
	default:
		break;
	}

	if (AnimationToPlay)
	{
		PlayAnimation(AnimationToPlay);
	}
}

void UMuseumQuizUI::HandleAnswerButton0Clicked()
{
	SubmitAnswer(0);
}

void UMuseumQuizUI::HandleAnswerButton1Clicked()
{
	SubmitAnswer(1);
}

void UMuseumQuizUI::HandleAnswerButton2Clicked()
{
	SubmitAnswer(2);
}

void UMuseumQuizUI::HandleAnswerButton3Clicked()
{
	SubmitAnswer(3);
}

void UMuseumQuizUI::HandleCloseButtonClicked()
{
	CloseQuizWidget();
}
