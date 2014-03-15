// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	PrecomputedLightVolume.h: Declarations for precomputed light volumes.
=============================================================================*/

#ifndef __PRECOMPUTEDLIGHTVOLUME_H__
#define __PRECOMPUTEDLIGHTVOLUME_H__

#include "GenericOctree.h"

/** Incident radiance stored for a point. */
class FVolumeLightingSample
{
public:
	/** World space position of the sample. */
	FVector Position;
	/** World space radius that determines how far the sample can be interpolated. */
	float Radius;

	/** 
	 * Incident lighting at the sample position. 
	 * For high quality lightmaps, only diffuse GI and static lights is represented here.
	 * For low quality lightmaps, diffuse GI and static and stationary light direct lighting is contained in the sample, with the exception of the stationary directional light. 
	 * @todo - quantize to reduce memory cost
	 */
	FSHVectorRGB2 Lighting;

	/** Shadow factor for the stationary directional light.  This is only valid in the low quality lightmap data. */
	float DirectionalLightShadowing;

	FVolumeLightingSample() :
		Position(FVector(0, 0, 0)),
		Radius(0),
		DirectionalLightShadowing(1)
	{
	}

	friend FArchive& operator<<(FArchive& Ar, FVolumeLightingSample& Sample);
};

struct FLightVolumeOctreeSemantics
{
	enum { MaxElementsPerLeaf = 4 };
	enum { MaxNodeDepth = 12 };

	/** Using the heap allocator instead of an inline allocator to trade off add/remove performance for memory. */
	typedef FDefaultAllocator ElementAllocator;

	FORCEINLINE static const float* GetBoundingBox(const FVolumeLightingSample& Sample)
	{
		FPlatformMisc::Prefetch( &Sample, CACHE_LINE_SIZE );
		// here we require that the position and radius are contiguous in memory
		checkAtCompileTime(STRUCT_OFFSET( FVolumeLightingSample, Position ) + 3 * sizeof(float) ==  STRUCT_OFFSET( FVolumeLightingSample, Radius ), FVolumeLightingSample_Radius_Must_Follow_Position );
		return &Sample.Position.X;
	}

	static void SetElementId(const FVolumeLightingSample& Element, FOctreeElementId Id)
	{
	}
};

typedef TOctree<FVolumeLightingSample, FLightVolumeOctreeSemantics> FLightVolumeOctree;

/** Set of volume lighting samples belonging to one streaming level, which can be queried about the lighting at a given position. */
class FPrecomputedLightVolume
{
public:

	ENGINE_API FPrecomputedLightVolume();
	~FPrecomputedLightVolume();

	friend FArchive& operator<<(FArchive& Ar, FPrecomputedLightVolume& Volume);

	ENGINE_API void AddToScene(FSceneInterface* Scene);

	ENGINE_API void RemoveFromScene(FSceneInterface* Scene);

	/** Frees any previous samples, prepares the volume to have new samples added. */
	ENGINE_API void Initialize(const FBox& NewBounds);

	/** Adds a high quality lighting sample. */
	ENGINE_API void AddHighQualityLightingSample(const FVolumeLightingSample& NewHighQualitySample);

	/** Adds a low quality lighting sample. */
	ENGINE_API void AddLowQualityLightingSample(const FVolumeLightingSample& NewLowQualitySample);

	/** Shrinks the octree and updates memory stats. */
	ENGINE_API void FinalizeSamples();

	/** Invalidates anything produced by the last lighting build. */
	ENGINE_API void InvalidateLightingCache();

	/** Interpolates incident radiance to Position. */
	ENGINE_API void InterpolateIncidentRadiancePoint(
		const FVector& Position, 
		float& AccumulatedWeight,
		float& AccumulatedDirectionalLightShadowing,
		FSHVectorRGB2& AccumulatedIncidentRadiance) const;
	
	/** Interpolates incident radiance to Position. */
	ENGINE_API void InterpolateIncidentRadianceBlock(
		const FBoxCenterAndExtent& BoundingBox, 
		const FIntVector& QueryCellDimensions,
		const FIntVector& DestCellDimensions,
		const FIntVector& DestCellPosition,
		TArray<float>& AccumulatedWeights,
		TArray<FSHVectorRGB2>& AccumulatedIncidentRadiance) const;

	ENGINE_API void DebugDrawSamples(FPrimitiveDrawInterface* PDI, bool bDrawDirectionalShadowing) const;

	SIZE_T GetAllocatedBytes() const;

	bool IsInitialized() const
	{
		return bInitialized;
	}

	bool IsAddedToScene() const
	{
		return bAddedToScene;
	}

	FBox& GetBounds()
	{
		return Bounds;
	}

	float GetNodeLevelExtent(int32 Level) const
	{
		return OctreeForRendering->GetNodeLevelExtent(Level);
	}

	void ApplyWorldOffset(const FVector& InOffset, bool bWorldShift);

private:

	bool bInitialized;
	bool bAddedToScene;
	FBox Bounds;

	/** Octree containing lighting samples to be used with high quality lightmaps. */
	FLightVolumeOctree HighQualityLightmapOctree;

	/** Octree containing lighting samples to be used with low quality lightmaps. */
	FLightVolumeOctree LowQualityLightmapOctree;

	/** Octree used to accelerate interpolation searches. */
	FLightVolumeOctree* OctreeForRendering;
};

#endif