// Fill out your copyright notice in the Description page of Project Settings.


#include "AGCHUD.h"

#include <Engine/Canvas.h>

void AAGCHUD::DrawHUD()
{
    Super::DrawHUD();

    FVector2D CenterP = 0.5f * FVector2D(Canvas->ClipX - Canvas->OrgX, Canvas->ClipY - Canvas->OrgY);
    const float CrossHairSize = 20.0f;

    Canvas->K2_DrawLine(CenterP - FVector2D(CrossHairSize, 0.0f), CenterP + FVector2D(CrossHairSize, 0.0f));
    Canvas->K2_DrawLine(CenterP - FVector2D(0.0f, CrossHairSize), CenterP + FVector2D(0.0f, CrossHairSize));
}