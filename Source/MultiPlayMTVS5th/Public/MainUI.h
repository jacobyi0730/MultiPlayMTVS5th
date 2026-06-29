// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYMTVS5TH_API UMainUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image_Crosshair;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> BulletPanel;
	
	UPROPERTY(EditAnywhere, Category = "Bullet")
	TSubclassOf<class UUserWidget> BulletUIFactory;
	
	void AddBullet();
	void PopBullet(int32 index);
	
	
	void SetActiveCrosshair(bool isActive);
};
