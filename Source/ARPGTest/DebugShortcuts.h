#pragma once
#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(location) if(GetWorld()) DrawDebugSphere(GetWorld(),location,100.f,24,FColor::Red,true);
#define DRAW_SPHERE_COLOR(location, Color) if(GetWorld()) DrawDebugSphere(GetWorld(),location,100.f,24,Color,true);
#define DRAW_SPHERE_COLOR_SIZE(location, Color, Size) if(GetWorld()) DrawDebugSphere(GetWorld(),location,Size,24,Color,true);

#define DRAW_SPHERE_TEMP(location,lifeTime) if(GetWorld()) DrawDebugSphere(GetWorld(),location,100.f,24,FColor::Red,false,lifeTime);
#define DRAW_SPHERE_TEMP_COLOR(location,Color,lifeTime) if(GetWorld()) DrawDebugSphere(GetWorld(),location,100.f,24,Color,false,lifeTime);
#define DRAW_SPHERE_TEMP_COLOR_SIZE(location,Color,Size,lifeTime) if(GetWorld()) DrawDebugSphere(GetWorld(),location,Size,24,Color,false,lifeTime);

#define DRAW_LINE(start, end) if(GetWorld()) DrawDebugLine(GetWorld(),start,end, FColor::Red,true,-1.f,0,2.f);
#define DRAW_LINE_TEMP(start, end, lifeTime) if(GetWorld()) DrawDebugLine(GetWorld(),start,end, FColor::Red,false,lifeTime,0,2.f);
#define DRAW_POINT(location) if(GetWorld()) DrawDebugPoint(GetWorld(),location, 15.f, FColor::Red, true);
#define DRAW_POINT_TEMP(location,lifeTime) if(GetWorld()) DrawDebugPoint(GetWorld(),location, 15.f, FColor::Red, false,lifeTime);

