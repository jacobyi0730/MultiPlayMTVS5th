// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/UniformGridPanel.h"

void UMainUI::UpdateHPBar(int32 Cur, int32 Max)
{
	float hpPer = static_cast<float>(Cur) / Max;
	HPBar->SetPercent(hpPer);
}

void UMainUI::AddBullet()
{
	auto bulletUI = CreateWidget(this, BulletUIFactory);
	
	BulletPanel->AddChildToUniformGrid(bulletUI, 0, BulletPanel->GetChildrenCount());
}

void UMainUI::PopBullet(int32 index)
{
	BulletPanel->RemoveChildAt(index);
}

void UMainUI::RemoveAllBullets()
{
	for (auto bullet : BulletPanel->GetAllChildren())
	{
		BulletPanel->RemoveChild(bullet);
	}
}

void UMainUI::SetActiveCrosshair(bool isActive)
{
	if (isActive)
	{
		Image_Crosshair->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Image_Crosshair->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainUI::PlayDamageAnim()
{
	PlayAnimation(DamageAnim);
}
