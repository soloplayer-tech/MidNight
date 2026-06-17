#include "Museum/UI/MuseumExplainUI.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMuseumExplainUI::SetArtworkName(FName InArtworkName)
{
	ArtworkName = InArtworkName;
	RefreshWidgetState();
}

void UMuseumExplainUI::SetExplanationText(const FString& InExplanationText)
{
	ExplanationText = InExplanationText;
	ErrorText.Reset();
	RefreshWidgetState();
}

void UMuseumExplainUI::SetErrorText(const FString& InErrorText)
{
	ErrorText = InErrorText;
	ExplanationText.Reset();
	RefreshWidgetState();
}

void UMuseumExplainUI::SetWidgetVisibility(bool bVisible)
{
	SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UMuseumExplainUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (CloseButton)
	{
		CloseButton->OnClicked.Clear();
		CloseButton->OnClicked.AddDynamic(this, &UMuseumExplainUI::HandleCloseButtonClicked);
	}

	RefreshWidgetState();
}

void UMuseumExplainUI::HandleCloseButtonClicked()
{
	SetWidgetVisibility(false);
}

void UMuseumExplainUI::RefreshWidgetState()
{
	if (ArtworkNameText)
	{
		ArtworkNameText->SetText(FText::FromName(ArtworkName));
	}

	if (ExplanationTextBlock)
	{
		ExplanationTextBlock->SetText(FText::FromString(ExplanationText));
	}

	if (ErrorTextBlock)
	{
		ErrorTextBlock->SetText(FText::FromString(ErrorText));
		ErrorTextBlock->SetVisibility(ErrorText.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
}
