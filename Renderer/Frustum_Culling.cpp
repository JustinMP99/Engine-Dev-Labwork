#pragma once

#include "frustum_culling.h"

float FOV = 1.0f;
float ViewRatio = 1.0f;
DirectX::XMFLOAT4 nearDist = { 2.0f, 2.0f, 2.0f, 2.0f };
DirectX::XMFLOAT4  farDist = { 25.0f, 25.0f, 25.0f, 25.0f };


end::plane_t end::calculate_plane(float3 a, float3 b, float3 c)
{
	//capture points into XMVECTOR
	DirectX::XMVECTOR P0 = { a.x, a.y, a.z };
	DirectX::XMVECTOR P1 = { b.x, b.y, b.z };
	DirectX::XMVECTOR P2 = { c.x, c.y, c.z };
	//calculate plane normal
	DirectX::XMVECTOR PlaneNormal  = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(P1, P0), DirectX::XMVectorSubtract(P2, P1));
	//Normalize result
	PlaneNormal = DirectX::XMVector3Normalize(PlaneNormal);
	//calculate offset
	float3 PlaneNormFloat = { DirectX::XMVectorGetX(PlaneNormal), DirectX::XMVectorGetY(PlaneNormal), DirectX::XMVectorGetZ(PlaneNormal)};
	float PlaneOffset;
	PlaneOffset = dot(PlaneNormFloat, a);

	plane_t Plane;
	Plane.normal = {DirectX::XMVectorGetX(PlaneNormal), DirectX::XMVectorGetY(PlaneNormal), DirectX::XMVectorGetZ(PlaneNormal) };
	Plane.offset = PlaneOffset;
	return Plane;
	//return plane_t();
}

void end::calculate_frustum(frustum_t & frustum, const view_t & view)
{
	DirectX::XMFLOAT4 XVec   = { DirectX::XMVectorGetX(view.FrustumMatrix.r[0]), DirectX::XMVectorGetY(view.FrustumMatrix.r[0]),  DirectX::XMVectorGetZ(view.FrustumMatrix.r[0]),  DirectX::XMVectorGetW(view.FrustumMatrix.r[0]) };
	DirectX::XMFLOAT4 YVec   = { DirectX::XMVectorGetX(view.FrustumMatrix.r[1]), DirectX::XMVectorGetY(view.FrustumMatrix.r[1]),  DirectX::XMVectorGetZ(view.FrustumMatrix.r[1]),  DirectX::XMVectorGetW(view.FrustumMatrix.r[1]) };
	DirectX::XMFLOAT4 ZVec   = { DirectX::XMVectorGetX(view.FrustumMatrix.r[2]), DirectX::XMVectorGetY(view.FrustumMatrix.r[2]),  DirectX::XMVectorGetZ(view.FrustumMatrix.r[2]),  DirectX::XMVectorGetW(view.FrustumMatrix.r[2]) };
	DirectX::XMFLOAT4 PosVec = { DirectX::XMVectorGetX(view.FrustumMatrix.r[3]), DirectX::XMVectorGetY(view.FrustumMatrix.r[3]),  DirectX::XMVectorGetZ(view.FrustumMatrix.r[3]),  DirectX::XMVectorGetW(view.FrustumMatrix.r[3]) };

	//calculate Near Center
	DirectX::XMFLOAT4 NearCenter = {
		PosVec.x + ZVec.x * nearDist.x,
		PosVec.y + ZVec.y * nearDist.y,
		PosVec.z + ZVec.z * nearDist.z,
		PosVec.w + ZVec.w * nearDist.w
	};
	//Calculate Far Center
	DirectX::XMFLOAT4 FarCenter = {
		PosVec.x + ZVec.x * farDist.x,
		PosVec.y + ZVec.y * farDist.y,
		PosVec.z + ZVec.z * farDist.z,
		PosVec.w + ZVec.w * farDist.w,
	};
	//Height calculations
	float Hnear = 2 * tan(FOV / 2) * nearDist.x;
	float Hfar = 2 * tan(FOV / 2) * farDist.x;
	//Width calculations
	float Wnear = Hnear * ViewRatio;
	float Wfar = Hfar * ViewRatio;

	//=======================================================================================
	//calculate near points 
	//=======================================================================================
	//Near - Top - Left
	DirectX::XMFLOAT4 NTL = {
		NearCenter.x + YVec.x * (Hnear * 0.5f) - XVec.x * (Wnear * 0.5f),
		NearCenter.y + YVec.y * (Hnear * 0.5f) - XVec.y * (Wnear * 0.5f),
		NearCenter.z + YVec.z * (Hnear * 0.5f) - XVec.z * (Wnear * 0.5f),
		NearCenter.w + YVec.w * (Hnear * 0.5f) - XVec.w * (Wnear * 0.5f),
	};
	//Near - Top - Right
	DirectX::XMFLOAT4 NTR = {
		NearCenter.x + YVec.x * (Hnear * 0.5f) + XVec.x * (Wnear * 0.5f),
		NearCenter.y + YVec.y * (Hnear * 0.5f) + XVec.y * (Wnear * 0.5f),
		NearCenter.z + YVec.z * (Hnear * 0.5f) + XVec.z * (Wnear * 0.5f),
		NearCenter.w + YVec.w * (Hnear * 0.5f) + XVec.w * (Wnear * 0.5f),
	};
	//Near - Bottom - Left
	DirectX::XMFLOAT4 NBL = {
		NearCenter.x - YVec.x * (Hnear * 0.5f) - XVec.x * (Wnear * 0.5f),
		NearCenter.y - YVec.y * (Hnear * 0.5f) - XVec.y * (Wnear * 0.5f),
		NearCenter.z - YVec.z * (Hnear * 0.5f) - XVec.z * (Wnear * 0.5f),
		NearCenter.w - YVec.w * (Hnear * 0.5f) - XVec.w * (Wnear * 0.5f),
	};
	//Near - Bottom - Right
	DirectX::XMFLOAT4 NBR = {
		NearCenter.x - YVec.x * (Hnear * 0.5f) + XVec.x * (Wnear * 0.5f),
		NearCenter.y - YVec.y * (Hnear * 0.5f) + XVec.y * (Wnear * 0.5f),
		NearCenter.z - YVec.z * (Hnear * 0.5f) + XVec.z * (Wnear * 0.5f),
		NearCenter.w - YVec.w * (Hnear * 0.5f) + XVec.w * (Wnear * 0.5f),
	};



	//=======================================================================================
	////calculate Far points 
	//=======================================================================================
	//Far - Top - Left
	DirectX::XMFLOAT4 FTL = {
		FarCenter.x + YVec.x * (Hfar * 0.5f) - XVec.x * (Wfar * 0.5f),
		FarCenter.y + YVec.y * (Hfar * 0.5f) - XVec.y * (Wfar * 0.5f),
		FarCenter.z + YVec.z * (Hfar * 0.5f) - XVec.z * (Wfar * 0.5f),
		FarCenter.w + YVec.w * (Hfar * 0.5f) - XVec.w * (Wfar * 0.5f),
	};
	//Far - Top - Right
	DirectX::XMFLOAT4 FTR = {
		FarCenter.x + YVec.x * (Hfar * 0.5f) + XVec.x * (Wfar * 0.5f),
		FarCenter.y + YVec.y * (Hfar * 0.5f) + XVec.y * (Wfar * 0.5f),
		FarCenter.z + YVec.z * (Hfar * 0.5f) + XVec.z * (Wfar * 0.5f),
		FarCenter.w + YVec.w * (Hfar * 0.5f) + XVec.w * (Wfar * 0.5f),
	};
	//Far - Bottom - Left
	DirectX::XMFLOAT4 FBL = {
		FarCenter.x - YVec.x * (Hfar * 0.5f) - XVec.x * (Wfar * 0.5f),
		FarCenter.y - YVec.y * (Hfar * 0.5f) - XVec.y * (Wfar * 0.5f),
		FarCenter.z - YVec.z * (Hfar * 0.5f) - XVec.z * (Wfar * 0.5f),
		FarCenter.w - YVec.w * (Hfar * 0.5f) - XVec.w * (Wfar * 0.5f),
	};
	//Far - Bottom - Right
	DirectX::XMFLOAT4 FBR = {
		FarCenter.x - YVec.x * (Hfar * 0.5f) + XVec.x * (Wfar * 0.5f),
		FarCenter.y - YVec.y * (Hfar * 0.5f) + XVec.y * (Wfar * 0.5f),
		FarCenter.z - YVec.z * (Hfar * 0.5f) + XVec.z * (Wfar * 0.5f),
		FarCenter.w - YVec.w * (Hfar * 0.5f) + XVec.w * (Wfar * 0.5f),
	};

	//=======================================================================================
	////Add points to debug renderer
	//=======================================================================================
	//near plane Top
	debug_renderer::add_line({ NTL.x, NTL.y, NTL.z }, { NTR.x, NTR.y, NTR.z }, { 1.0f,1.0f, 0.0f, 0.0f });
	//near plane Bottom
	debug_renderer::add_line({ NBL.x, NBL.y, NBL.z }, { NBR.x, NBR.y, NBR.z }, { 1.0f,1.0f, 0.0f, 0.0f });
	//near plane Left
	debug_renderer::add_line({ NTL.x, NTL.y, NTL.z }, { NBL.x, NBL.y, NBL.z }, { 1.0f,1.0f, 0.0f, 0.0f });
	//near plane Right
	debug_renderer::add_line({ NTR.x, NTR.y, NTR.z }, { NBR.x, NBR.y, NBR.z }, { 1.0f,1.0f, 0.0f, 0.0f });

	//far plane top
	debug_renderer::add_line({ FTL.x, FTL.y, FTL.z }, { FTR.x, FTR.y, FTR.z }, { 1.0f,1.0f, 0.0f, 0.0f });
	//far plane bottom
	debug_renderer::add_line({ FBL.x, FBL.y, FBL.z }, { FBR.x, FBR.y, FBR.z }, { 1.0f,1.0f, 0.0f, 0.0f });
	//far plane left
	debug_renderer::add_line({ FTL.x, FTL.y, FTL.z }, { FBL.x, FBL.y, FBL.z }, { 1.0f,1.0f, 0.0f, 0.0f });
	//far plane right
	debug_renderer::add_line({ FTR.x, FTR.y, FTR.z }, { FBR.x, FBR.y, FBR.z }, { 1.0f,1.0f, 0.0f, 0.0f });

	//Frustum right side connecting lines 
	//top
	debug_renderer::add_line({ NTR.x, NTR.y, NTR.z }, { FTR.x, FTR.y, FTR.z }, { 1.0f,1.0f, 0.0f, 0.0f });
	//bottom
	debug_renderer::add_line({ NBR.x, NBR.y, NBR.z }, { FBR.x, FBR.y, FBR.z }, { 1.0f,1.0f, 0.0f, 0.0f });

	//Frustum Left side Connecting lines
	//top
	debug_renderer::add_line({ NTL.x, NTL.y, NTL.z }, { FTL.x, FTL.y, FTL.z }, { 1.0f,1.0f, 0.0f, 0.0f });
	//bottom
	debug_renderer::add_line({ NBL.x, NBL.y, NBL.z }, { FBL.x, FBL.y, FBL.z }, { 1.0f,1.0f, 0.0f, 0.0f });



	//=======================================================================================
	////Calculate Plane Normal and offset
	//=======================================================================================
	
	//Top Plane
	//plane_t TopPlane = calculate_plane({NTL.x, NTL.y, NTL.z}, {FTL.x, FTL.y, FTL.z}, { FTR.x, FTR.y, FTR.z });
	plane_t TopPlane = calculate_plane({ FTR.x, FTR.y, FTR.z }, { FTL.x, FTL.y, FTL.z }, { NTL.x, NTL.y, NTL.z });
	//Bottom plane
	plane_t BottomPlane = calculate_plane({NBL.x, NBL.y, NBL.z}, {FBL.x, FBL.y, FBL.z}, {FBR.x, FBR.y, FBR.z});
	
	//Left plane
	plane_t LeftPlane = calculate_plane({ FTL.x, FTL.y, FTL.z }, { FBL.x, FBL.y, FBL.z }, { NBL.x, NBL.y, NBL.z });
	//plane_t LeftPlane = calculate_plane({NBL.x, NBL.y, NBL.z}, {FBL.x, FBL.y, FBL.z}, {FTL.x, FTL.y, FTL.z});
	//Right plane
	plane_t RightPlane = calculate_plane({NBR.x, NBR.y, NBR.z}, {FBR.x, FBR.y, FBR.z}, { FTR.x, FTR.y, FTR.z });
	
	//far plane
	plane_t FarPlane = calculate_plane({ FBR.x, FBR.y, FBR.z }, { FBL.x, FBL.y, FBL.z }, { FTL.x, FTL.y, FTL.z });
	//near plane
	plane_t NearPlane = calculate_plane({ NTL.x, NTL.y, NTL.z }, { NBL.x, NBL.y, NBL.z }, { NBR.x, NBR.y, NBR.z });


	frustum[0].normal = TopPlane.normal;
	frustum[0].offset = TopPlane.offset;
	
	frustum[1].normal = BottomPlane.normal;
	frustum[1].offset = BottomPlane.offset;
	
	frustum[2].normal = LeftPlane.normal;
	frustum[2].offset = LeftPlane.offset;
	
	frustum[3].normal = RightPlane.normal;
	frustum[3].offset = RightPlane.offset;
	
	frustum[4].normal = FarPlane.normal;
	frustum[4].offset = FarPlane.offset;
	
	frustum[5].normal = NearPlane.normal;
	frustum[5].offset = NearPlane.offset;

	//=======================================================================================
	//calculate planes normal center point(Top, Bottom, Left, Right)
	//=======================================================================================
	//Top plane center
	DirectX::XMFLOAT4 TopCenterPoint = {
		(NTL.x + FTL.x + NTR.x + FTR.x) / 4,
		(NTL.y + FTL.y + NTR.y + FTR.y) / 4,
		(NTL.z + FTL.z + NTR.z + FTR.z) / 4,
		(NTL.w + FTL.w + NTR.w + FTR.w) / 4,
	};
	//Bottom plane center
	DirectX::XMFLOAT4 BottomCenterPoint = {
		(NBL.x + FBL.x + NBR.x + FBR.x) / 4,
		(NBL.y + FBL.y + NBR.y + FBR.y) / 4,
		(NBL.z + FBL.z + NBR.z + FBR.z) / 4,
		(NBL.w + FBL.w + NBR.w + FBR.w) / 4,
	};
	//Left plane center
	DirectX::XMFLOAT4 LeftCenterPoint = {
		(NTL.x + FTL.x + NBL.x + FBL.x) / 4,
		(NTL.y + FTL.y + NBL.y + FBL.y) / 4,
		(NTL.z + FTL.z + NBL.z + FBL.z) / 4,
		(NTL.w + FTL.w + NBL.w + FBL.w) / 4,
	};
	//Right plane center
	DirectX::XMFLOAT4 RightCenterPoint = {
		(NTR.x + FTR.x + NBR.x + FBR.x) / 4,
		(NTR.y + FTR.y + NBR.y + FBR.y) / 4,
		(NTR.z + FTR.z + NBR.z + FBR.z) / 4,
		(NTR.w + FTR.w + NBR.w + FBR.w) / 4,
	};
	//Far plane center
	DirectX::XMFLOAT4 FarCenterPoint = {
		(FTL.x + FTR.x + FBL.x + FBR.x) / 4,
		(FTL.y + FTR.y + FBL.y + FBR.y) / 4,
		(FTL.z + FTR.z + FBL.z + FBR.z) / 4,
		(FTL.w + FTR.w + FBL.w + FBR.w) / 4,
	};
	//Near plane center
	DirectX::XMFLOAT4 NearCenterPoint = {
		(NTL.x + NTR.x + NBL.x + NBR.x) / 4,
		(NTL.y + NTR.y + NBL.y + NBR.y) / 4,
		(NTL.z + NTR.z + NBL.z + NBR.z) / 4,
		(NTL.w + NTR.w + NBL.w + NBR.w) / 4,
	};

	//=======================================================================================
	//calculate planes normal end point(Top, Bottom, Left, Right)
	//=======================================================================================
	
	//Top center end
	DirectX::XMFLOAT4 TopCenterEndPoint = {
		TopCenterPoint.x + (TopPlane.normal.x * 3.25f),
		TopCenterPoint.y + (TopPlane.normal.y * 3.25f),
		TopCenterPoint.z + (TopPlane.normal.z * 3.25f),
		TopCenterPoint.w
	};
	//Bottom center end
	DirectX::XMFLOAT4 BottomCenterEndPoint = {
		BottomCenterPoint.x + (BottomPlane.normal.x * 1.25f),
		BottomCenterPoint.y + (BottomPlane.normal.y * 1.25f),
		BottomCenterPoint.z + (BottomPlane.normal.z * 1.25f),
		BottomCenterPoint.w
	};
	//Left center end
	DirectX::XMFLOAT4 LeftCenterEndPoint = {
		LeftCenterPoint.x + (LeftPlane.normal.x * 1.25f),
		LeftCenterPoint.y + (LeftPlane.normal.y * 1.25f),
		LeftCenterPoint.z + (LeftPlane.normal.z * 1.25f),
		LeftCenterPoint.w
	};
	//Right center end
	DirectX::XMFLOAT4 RightCenterEndPoint = {
		RightCenterPoint.x + (RightPlane.normal.x * 1.25f),
		RightCenterPoint.y + (RightPlane.normal.y * 1.25f),
		RightCenterPoint.z + (RightPlane.normal.z * 1.25f),
		RightCenterPoint.w
	};

	//Far center end
	DirectX::XMFLOAT4 FarCenterEndPoint = {
		FarCenterPoint.x + (FarPlane.normal.x * 1.25f),
		FarCenterPoint.y + (FarPlane.normal.y * 1.25f),
		FarCenterPoint.z + (FarPlane.normal.z * 1.25f),
		FarCenterPoint.w
	};
	//Far center end
	DirectX::XMFLOAT4 NearCenterEndPoint = {
		NearCenterPoint.x + (NearPlane.normal.x * 1.25f),
		NearCenterPoint.y + (NearPlane.normal.y * 1.25f),
		NearCenterPoint.z + (NearPlane.normal.z * 1.25f),
		NearCenterPoint.w
	};

	//=======================================================================================
	//draw frustum face normals
	//=======================================================================================
	//Top Normal
	debug_renderer::add_line({ TopCenterPoint.x, TopCenterPoint.y, TopCenterPoint.z }, { TopCenterEndPoint.x, TopCenterEndPoint.y, TopCenterEndPoint.z }, { 1.0f,0.0f,0.0f,1.0f });
	//Bottom Normal
	debug_renderer::add_line({ BottomCenterPoint.x, BottomCenterPoint.y, BottomCenterPoint.z }, { BottomCenterEndPoint.x, BottomCenterEndPoint.y, BottomCenterEndPoint.z }, { 1.0f,0.0f,0.0f,1.0f });
	//Left Normal
	debug_renderer::add_line({ LeftCenterPoint.x, LeftCenterPoint.y, LeftCenterPoint.z }, { LeftCenterEndPoint.x, LeftCenterEndPoint.y, LeftCenterEndPoint.z }, { 1.0f,0.0f,0.0f,1.0f });
	//Right Normal
	debug_renderer::add_line({ RightCenterPoint.x, RightCenterPoint.y, RightCenterPoint.z }, { RightCenterEndPoint.x, RightCenterEndPoint.y, RightCenterEndPoint.z }, { 1.0f,0.0f,0.0f,1.0f });
	//Far normal
	debug_renderer::add_line({   FarCenterPoint.x,   FarCenterPoint.y,   FarCenterPoint.z }, {   FarCenterEndPoint.x,   FarCenterEndPoint.y,   FarCenterEndPoint.z }, { 1.0f,0.0f,0.0f,1.0f });
	//Near normal
	debug_renderer::add_line({ NearCenterPoint.x, NearCenterPoint.y, NearCenterPoint.z }, { NearCenterEndPoint.x, NearCenterEndPoint.y, NearCenterEndPoint.z }, { 1.0f,0.0f,0.0f,1.0f });

}

int end::classify_sphere_to_plane(const sphere_t & sphere, const plane_t & plane)
{
	//calculate  signed distance of sphere center to plane
	/*float3 cap = { 
		(sphere.center.x * plane.normal.x) - plane.offset,
		(sphere.center.y * plane.normal.y) - plane.offset,
		(sphere.center.z * plane.normal.z) - plane.offset
	};*/
	
	float3 sphereCenter = { sphere.center.x, sphere.center.y, sphere.center.z };
	float3 planeNormal  = {  plane.normal.x,  plane.normal.y,  plane.normal.z };


	float cap = dot(sphereCenter,planeNormal) - plane.offset;
	if (cap > sphere.radius)
	{
		return 1;
	}
	if (cap < -sphere.radius)
	{
		return -1;
	}

	return 0;
}

int end::classify_aabb_to_plane(const aabb_t & aabb, const plane_t & plane)
{
	//capture plane normal and aabb extent for later use
	float3 planeNormal = { plane.normal.x,  plane.normal.y,  plane.normal.z };
	float3 Extents = {aabb.extents.x, aabb.extents.y, aabb.extents.z};
	//create sphere
	sphere_t sphere;
	sphere.center = aabb.center;
	sphere.radius = Extents.x * abs(planeNormal.x) + Extents.y * abs(planeNormal.y) + Extents.z * abs(planeNormal.z);


	return classify_sphere_to_plane(sphere, plane);
}

bool end::aabb_to_frustum(const aabb_t & aabb, const frustum_t & frustum)
{
	for (int i = 0; i < frustum.size(); i++)
	{
		if (classify_aabb_to_plane(aabb, frustum[i]) == -1)
		{
			return false;
		}
	}
	return true;
}
