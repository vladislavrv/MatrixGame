// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "stdafx.h"
#include "MatrixMap.hpp"
#include "MatrixLoadProgress.hpp"
#include "common.hpp"
#include "MatrixObjectBuilding.hpp"
#include "MatrixTerSurface.hpp"
#include "MatrixObjectCannon.hpp"
#include "MatrixObject.hpp"
#include "MatrixRobot.hpp"
#include "MatrixFlyer.hpp"
#include "MatrixShadowManager.hpp"
#include "ShadowStencil.hpp"
#include "Interface/CConstructor.h"

void CMatrixMap::PointCalcNormals(int x,int y)
{
    DTRACE();

	SMatrixMapPoint *p0; // cur
	SMatrixMapPoint *p1; // up
	SMatrixMapPoint *p2; // right
	SMatrixMapPoint *p3; // down
	SMatrixMapPoint *p4; // left

	D3DXVECTOR3 pv0;
	D3DXVECTOR3 pv1;
	D3DXVECTOR3 pv2;
	D3DXVECTOR3 pv3;
	D3DXVECTOR3 pv4;

	p0=PointGetTest(x,y);
	if(p0==NULL) return;

	SMatrixMapUnit * mu;

	mu=UnitGetTest(x,y-1);
	if(mu==NULL || !mu->IsLand()) p1=NULL; else p1=PointGetTest(x,y-1);
	mu=UnitGetTest(x-1,y);
	if(mu==NULL || !mu->IsLand()) p4=NULL; else p4=PointGetTest(x-1,y);

	mu=UnitGetTest(x,y);
	if(mu==NULL || !mu->IsLand())
    {
		p2=NULL;
		p3=NULL;
	} else {
		p2=PointGetTest(x+1,y);
		p3=PointGetTest(x,y+1);
	}

    // calculate 3D vectors of landscape cells (center point is pv0)

	pv0=D3DXVECTOR3(GLOBAL_SCALE*(x),GLOBAL_SCALE*(y),float(p0->z));
	if(p1!=NULL) pv1=D3DXVECTOR3(GLOBAL_SCALE*(x)-pv0.x,GLOBAL_SCALE*(y-1)-pv0.y,float(p1->z)-pv0.z);
	if(p2!=NULL) pv2=D3DXVECTOR3(GLOBAL_SCALE*(x+1)-pv0.x,GLOBAL_SCALE*(y)-pv0.y,float(p2->z)-pv0.z);
	if(p3!=NULL) pv3=D3DXVECTOR3(GLOBAL_SCALE*(x)-pv0.x,GLOBAL_SCALE*(y+1)-pv0.y,float(p3->z)-pv0.z);
	if(p4!=NULL) pv4=D3DXVECTOR3(GLOBAL_SCALE*(x-1)-pv0.x,GLOBAL_SCALE*(y)-pv0.y,float(p4->z)-pv0.z);

	int cnt=0;
	D3DXVECTOR3 n(0,0,0),nc;
	if(p1!=NULL && p2!=NULL)
    {
		D3DXVec3Cross(&nc,&pv1,&pv2);
		D3DXVec3Normalize(&nc,&nc);
		n+=nc;
		cnt++;
	}
	if(p2!=NULL && p3!=NULL)
    {
		D3DXVec3Cross(&nc,&pv2,&pv3);
		D3DXVec3Normalize(&nc,&nc);
		n+=nc;
		cnt++;
	}
	if(p3!=NULL && p4!=NULL)
    {
		D3DXVec3Cross(&nc,&pv3,&pv4);
		D3DXVec3Normalize(&nc,&nc);
		n+=nc;
		cnt++;
	}
	if(p4!=NULL && p1!=NULL)
    {
		D3DXVec3Cross(&nc,&pv4,&pv1);
		D3DXVec3Normalize(&nc,&nc);
		n+=nc;
		cnt++;
	}

	if(cnt==0)
    {
		if(p1!=NULL) p0->n=p1->n;
		else if(p4!=NULL) p0->n=p4->n;
		else p0->n=D3DXVECTOR3(0,0,1);
	} else
    {
		//p0->n=n/float(cnt);
        D3DXVec3Normalize(&p0->n, &n);
	}
}


static int BuildTexUnions(CStorage &stor, int lp1, int lp2)
{
    CDataBuf *tuc = stor.GetBuf(DATA_TEXUINIONS, DATA_TEXUINIONS_DATA, ST_BYTE);
    if (tuc == NULL) return -1;
    int cnt = tuc->GetArraysCount();
    if (cnt <= 0) return -1;

    CDataBuf *botc = stor.GetBuf(DATA_BOTTOM, DATA_BOTTOM_DATA, ST_INT32);
    if (botc == NULL) return -1;
    CDataBuf *bmpc = stor.GetBuf(DATA_BITMAPS, DATA_BITMAPS_BITMAP, ST_BYTE);
    if (bmpc == NULL) return -1;

    CBottomTextureUnion::Init(cnt);

    CBitmap bmp(g_CacheHeap);
    bmp.CreateRGB((TEX_BOTTOM_SIZE * g_MatrixMap->m_TexUnionDim),(TEX_BOTTOM_SIZE * g_MatrixMap->m_TexUnionDim));

    CBitmap **srcb = (CBitmap **)_alloca(sizeof(CBitmap *) * g_MatrixMap->IdsGetCount());
    memset(srcb, 0, sizeof(CBitmap *) * g_MatrixMap->IdsGetCount());

    CBitmap **inmapb = (CBitmap **)_alloca(sizeof(CBitmap *) * bmpc->GetArraysCount());
    memset(inmapb, 0, sizeof(CBitmap *) * bmpc->GetArraysCount());

    int lpc = cnt * g_MatrixMap->m_TexUnionSize;
    float deltalp = float(lp2-lp1) / float(lpc);
    float clp = float(lp1);


    for (int i=0; i<cnt; ++i)
    {
        if (i == (cnt-1))
        {
            bmp.Fill(CPoint(0,0), bmp.Size(), 0x00);
        }

        int *un = tuc->GetFirst<int>(i);

        int xx = 0;
        int yy = 0;
        int tsz = (TEX_BOTTOM_SIZE * g_MatrixMap->m_TexUnionDim);

        for (int k=0; k<g_MatrixMap->m_TexUnionSize; ++k)
        {
            g_LoadProgress->SetCurLPPos(Float2Int(clp));
            clp += deltalp;


            if (un[k] >= 0)
            {
                int bc = botc->GetArrayLength(un[k]);
                int *bot = botc->GetFirst<int>(un[k]); // get bottom texture info

                if (srcb[*bot] == NULL)
                {
                    // source bitmap not yet loaded
                    srcb[*bot] = HNew(g_CacheHeap) CBitmap(g_CacheHeap);
                    srcb[*bot]->LoadFromPNG(g_MatrixMap->IdsGet(*bot).Get());
                }
                bmp.Copy(CPoint(xx,yy), CPoint(TEX_BOTTOM_SIZE,TEX_BOTTOM_SIZE), *srcb[*bot], CPoint(0,0));
                --bc;
                ++bot;
                while (bc > 0)
                {
                    int ids = *bot++;
                    int ibm = *bot++;
                    bc -= 2;

                    if (inmapb[ibm] == NULL)
                    {
                        // source bitmap not yet loaded
                        inmapb[ibm] = HNew(g_CacheHeap) CBitmap(g_CacheHeap);
                        inmapb[ibm]->LoadFromPNG(bmpc->GetFirst<BYTE>(ibm), bmpc->GetArrayLength(ibm));
                    }

                    if (ids >= 0)
                    {
                        // load source and mix with mask
                        if (srcb[ids] == NULL)
                        {
                            // source bitmap not yet loaded
                            srcb[ids] = HNew(g_CacheHeap) CBitmap(g_CacheHeap);
                            srcb[ids]->LoadFromPNG(g_MatrixMap->IdsGet(ids).Get());
                        }

                        bmp.MergeByMask(CPoint(xx,yy),CPoint(TEX_BOTTOM_SIZE,TEX_BOTTOM_SIZE),
                            bmp,CPoint(xx,yy),*srcb[ids],CPoint(0,0),*inmapb[ibm],CPoint(0,0));

                    } else
                    {
                        // mix with bitmap as is
                        bmp.MergeWithAlpha(CPoint(xx,yy), CPoint(TEX_BOTTOM_SIZE,TEX_BOTTOM_SIZE),*inmapb[ibm], CPoint(0,0));
                    }

                }

            }

            xx += TEX_BOTTOM_SIZE;
            if (xx >= tsz)
            {
                yy += TEX_BOTTOM_SIZE;
                xx = 0;
            }
        }
        xx = 0;
        yy = 0;
        for (int k=0; k<g_MatrixMap->m_TexUnionSize; ++k)
        {
            if (un[k] < 0)
            {
                bool lp = (xx > 0) && un[k-1] >= 0;
                bool tp = (yy > 0) && un[k-g_MatrixMap->m_TexUnionDim] >= 0;
                bool rp = (xx < (tsz-TEX_BOTTOM_SIZE)) && un[k+1] >= 0;
                bool bp = (yy < (tsz-TEX_BOTTOM_SIZE)) && un[k+g_MatrixMap->m_TexUnionDim] >= 0;
                
                if (lp)
                {
                    int up = 0;
                    int down = TEX_BOTTOM_SIZE;
                    for (int u=0;u<(TEX_BOTTOM_SIZE/2-2);++u)
                    {
                        bmp.Copy(CPoint(xx+u,yy+up),CPoint(1,down-up),bmp,CPoint(xx-1,yy+up));
                        if (tp) ++up;
                        if (bp) --down;
                    }
                }
                if (tp)
                {
                    int left = 0;
                    int rite = TEX_BOTTOM_SIZE;
                    for (int u=0;u<(TEX_BOTTOM_SIZE/2-2);++u)
                    {
                        bmp.Copy(CPoint(xx+left,yy+u),CPoint(rite-left,1),bmp,CPoint(xx+left,yy-1));
                        if (lp) ++left;
                        if (rp) --rite;
                    }
                }
                if (rp)
                {
                    int up = 0;
                    int down = TEX_BOTTOM_SIZE;
                    for (int u=1;u<=(TEX_BOTTOM_SIZE/2-2);++u)
                    {
                        bmp.Copy(CPoint(xx+TEX_BOTTOM_SIZE-u,yy+up),CPoint(1,down-up),bmp,CPoint(xx+TEX_BOTTOM_SIZE,yy+up));
                        if (tp) ++up;
                        if (bp) --down;
                    }
                }
                if (bp)
                {
                    int left = 0;
                    int rite = TEX_BOTTOM_SIZE;
                    for (int u=1;u<=(TEX_BOTTOM_SIZE/2-2);++u)
                    {
                        bmp.Copy(CPoint(xx+left,yy+TEX_BOTTOM_SIZE-u),CPoint(rite-left,1),bmp,CPoint(xx+left,yy+TEX_BOTTOM_SIZE));
                        if (lp) ++left;
                        if (rp) --rite;
                    }
                }

            }
            xx += TEX_BOTTOM_SIZE;
            if (xx >= tsz)
            {
                yy += TEX_BOTTOM_SIZE;
                xx = 0;
            }

        }

        //bmp.SaveInPNG((CWStr(L"bla_") + i + L".png").Get());
        CBottomTextureUnion::Get(i).MakeFromBitmap(bmp);
    }


    for (int i=0; i<g_MatrixMap->IdsGetCount(); ++i)
    {
        if (srcb[i]) { HDelete(CBitmap, srcb[i], g_CacheHeap); }
    }
    for (DWORD i=0; i<bmpc->GetArraysCount(); ++i)
    {
        if (inmapb[i]) { HDelete(CBitmap, inmapb[i], g_CacheHeap); }
    }

    return 0;
}


void  SGroupVisibility::Release(void)
{
    if (vis)
    {
        HFree(vis, g_MatrixHeap);
        vis = NULL;
        vis_cnt = 0;
    }
    if (levels)
    {
        HFree(levels, g_MatrixHeap);
        levels = NULL;
        levels_cnt = 0;
    }
}


void CMatrixMap::ClearGroupVis(void)
{
	if(m_GroupVis!=NULL)
    {
		int cnt=m_GroupSize.x*m_GroupSize.y;
		SGroupVisibility * md=m_GroupVis;
		while(cnt>0)
        {
            md->Release();
			md++;
			cnt--;
		}
		HFree(m_GroupVis,g_MatrixHeap);
		m_GroupVis=NULL;
	}
}

void CMatrixMap::GroupClear(void)
{
    DTRACE();

    ClearGroupVis();

	if(m_Group!=NULL)
    {
		int cnt=m_GroupSize.x*m_GroupSize.y;
		CMatrixMapGroup * * md=m_Group;
		while(cnt>0)
        {
            if (*md)
            {
                HDelete(CMatrixMapGroup,*md,g_MatrixHeap);
            }
			md++;
			cnt--;
		}
		HFree(m_Group,g_MatrixHeap);
        HFree(m_VisibleGroups,g_MatrixHeap);

		m_Group=NULL;
	}
	m_GroupSize.x=0; m_GroupSize.y=0;
}

void CMatrixMap::GroupBuild(CStorage &stor)
{
    DTRACE();
	GroupClear();

	m_GroupSize.x = TruncFloat((m_Size.x + MAP_GROUP_SIZE - 1) * INVERT(MAP_GROUP_SIZE));
	m_GroupSize.y = TruncFloat((m_Size.y + MAP_GROUP_SIZE - 1) * INVERT(MAP_GROUP_SIZE));

    int cnt=m_GroupSize.x*m_GroupSize.y;

	m_Group=(CMatrixMapGroup **)HAllocClear(cnt * sizeof(CMatrixMapGroup **),g_MatrixHeap);
    m_VisibleGroups = (CMatrixMapGroup * *)HAlloc(cnt * sizeof(CMatrixMapGroup **),g_MatrixHeap);
	CMatrixMapGroup ** md = m_Group;

  //  for (int i=0; i<cnt; ++i)
  //  {
		//*md = HNew(g_MatrixHeap) CMatrixMapGroup();
  //  }
    
    CDataBuf *grpc = stor.GetBuf(DATA_GROUPS, DATA_GROUPS_DATA, ST_BYTE);
    for (DWORD i=0; i<grpc->GetArraysCount(); ++i)
    {
        BYTE *g = grpc->GetFirst<BYTE>(i);
        int x = *((WORD*)g); g+=2;
        int y = *((WORD*)g); g+=2;

        int disp = (x / MAP_GROUP_SIZE) + (y / MAP_GROUP_SIZE) * m_GroupSize.x;
        md[disp] = HNew(g_MatrixHeap) CMatrixMapGroup();
        md[disp]->BuildBottom(x,y, g);

    }


}

struct SPreRobot
{
    SSpecialBot sb;
    D3DXVECTOR3 pos;
    int         side;
    int         group; // группа 0,1,2,3 (0-без группы)
    float       angle;
};

int  CMatrixMap::ReloadDynamics(CStorage &stor, CMatrixMap::EReloadStep step, CBuf *robots)
{
    CDataBuf *propkey = stor.GetBuf(DATA_PROPERTIES, DATA_PROPERTIES_NAME, ST_WCHAR);
    CDataBuf *propval = stor.GetBuf(DATA_PROPERTIES, DATA_PROPERTIES_VALUE, ST_WCHAR);

    int ic;

    CMatrixMapStatic *ms;

    if (step == RS_SIDEAI)
    {
        m_MaintenancePRC = 100;
        ic = propkey->FindAsWStr(DATA_MAINTENANCETIME);
        if (ic >= 0)
        {
            m_MaintenancePRC = propval->GetAsWStr(ic).GetInt();
        }
        InitMaintenanceTime();

        ic = propkey->FindAsWStr(DATA_SIDEAIINFO);
        if (ic >= 0)
        {
            CWStr sideaiinfo(propval->GetAsWStr(ic), g_CacheHeap);
            CWStr inf(g_CacheHeap), na(g_CacheHeap), da(g_CacheHeap), data(g_CacheHeap);

            CBlockPar *bps = g_MatrixData->BlockGet(L"Side");
            int sc = bps->ParCount();

            int cnt = sideaiinfo.GetCountPar(L"|");
            for (int i=0;i<cnt;++i)
            {
                inf = sideaiinfo.GetStrPar(i,L"|");
                na = inf.GetStrPar(0,L":");
                da = inf.GetStrPar(1,L":");
                for (int s=0;s<sc;++s)
                {
                    if (bps->ParGet(s).GetStrPar(0,L",") == na)
                    {
                        CMatrixSideUnit *su = g_MatrixMap->GetSideById(bps->ParGetName(s).GetInt());

                        int cd = da.GetCountPar(L"/");
                        data = da;
                        for (int d=0;d<cd;d+=2)
                        {
                            na.Set(data.GetStrPar(d,L"/"));
                            da.Set(data.GetStrPar(d+1,L"/"));

                            if (na == L"TBB") su->m_TimeNextBomb = g_MatrixMap->GetTime() + da.GetInt();
                            else if (na == L"SK") su->m_StrengthMul = (float)da.GetDouble();
                            else if (na == L"DK") su->m_DangerMul = (float)da.GetDouble();
                            else if (na == L"WRK") su->m_WaitResMul = (float)da.GetDouble();
                            else if (na == L"BK") su->m_BraveMul = (float)da.GetDouble();
                            else if (na == L"TC") su->m_TeamCnt = da.GetInt();
                        }
                        break;
                    }
                }
            }
        }
        return 0;
    }

    if (step == RS_RESOURCES)
    {
        ic = propkey->FindAsWStr(DATA_SIDERESINFO);
        if (ic >= 0)
        {
            CWStr sideresinfo(g_CacheHeap);
            CWStr def(g_CacheHeap);
            sideresinfo = propval->GetAsWStr(ic);
            int cnt = sideresinfo.GetCountPar(L"|");
            for (int i=0; i<cnt;++i)
            {
                def = sideresinfo.GetStrPar(i, L"|");

                int id = def.GetIntPar(0, L",");

                for (int j=0; j<m_SideCnt; ++j)
                {
                    CMatrixSideUnit *su = m_Side + j;
                    if (su->m_Id == id)
                    {
                        for (int k=0; k<MAX_RESOURCES; ++k)
                        {
                            su->SetResourceAmount((ERes)k, def.GetIntPar(1 + k, L","));
                        }

                        if (def.GetCountPar(L",") > 5)
                        {
                            su->SetResourceForceUp(def.GetIntPar(5, L","));
                        } else
                        {
                            su->SetResourceForceUp(100);
                        }
                        break;
                    }
                }
            }
        }
        return 0;
    }

    // load mapobjects
    if (step == RS_MAPOBJECTS)
    {
        int ouid = 0;

        int n = 0;
        ic = propkey->FindAsWStr(DATA_SHADOWTAGCOUNT);
        if (ic >= 0)  n = propval->GetAsWStr(ic).GetInt();
        CMatrixMapObject::InitTextures(n);

        // load objects
        CDataBuf *c0 = stor.GetBuf(DATA_OBJECTS, DATA_OBJECTS_X, ST_FLOAT);
        CDataBuf *c1 = stor.GetBuf(DATA_OBJECTS, DATA_OBJECTS_Y, ST_FLOAT);
        CDataBuf *c2 = stor.GetBuf(DATA_OBJECTS, DATA_OBJECTS_ANGLE_Z, ST_FLOAT);
        CDataBuf *c2_x = stor.GetBuf(DATA_OBJECTS, DATA_OBJECTS_ANGLE_X, ST_FLOAT);
        CDataBuf *c2_y = stor.GetBuf(DATA_OBJECTS, DATA_OBJECTS_ANGLE_Y, ST_FLOAT);
        CDataBuf *c3 = stor.GetBuf(DATA_OBJECTS, DATA_OBJECTS_HEIGHT, ST_FLOAT);
        CDataBuf *c3_ = stor.GetBuf(DATA_OBJECTS, DATA_OBJECTS_Z, ST_FLOAT);
        CDataBuf *c4 = stor.GetBuf(DATA_OBJECTS, DATA_OBJECTS_SCALE, ST_FLOAT);
        CDataBuf *c6 = stor.GetBuf(DATA_OBJECTS, DATA_OBJECTS_TYPE, ST_DWORD);
        CDataBuf *c7 = stor.GetBuf(DATA_OBJECTS, DATA_OBJECTS_SHADOW, ST_BYTE);

        float *xf = c0->GetFirst<float>(0);
        float *yf = c1->GetFirst<float>(0);
        float *ang = c2->GetFirst<float>(0);
        float *ang_x = c2_x?c2_x->GetFirst<float>(0):NULL;
        float *ang_y = c2_y?c2_y->GetFirst<float>(0):NULL;
        float *he = c3?c3->GetFirst<float>(0):NULL;
        float *zf = c3_?c3_->GetFirst<float>(0):NULL;
        float *scale = c4->GetFirst<float>(0);
        int *type = c6->GetFirst<INT32>(0);

        n = c0->GetArrayLength(0);
        for (int i=0;i<n; ++i)
        {
            CMatrixMapObject *o;
            o = StaticAdd<CMatrixMapObject>(false);

            SObjectCore *co = o->GetCore(DEBUG_CALL_INFO);

            co->m_Matrix._41 = xf[i];
            co->m_Matrix._42 = yf[i];

            o->m_AngleZ = ang[i];
            if (ang_x)
            {
                o->m_AngleX = ang_x[i];
                o->m_AngleY = ang_y[i];
            }

            if (he)
            {
	            int px = TruncFloat(xf[i] * INVERT(GLOBAL_SCALE));
	            int py = TruncFloat(yf[i] * INVERT(GLOBAL_SCALE));

                int cnt=0;
                float rv=0;
                SMatrixMapPoint * mp=g_MatrixMap->PointGetTest(px,py); if(mp) { cnt++; rv+=mp->z; }
                mp=g_MatrixMap->PointGetTest(px+1,py); if(mp) { cnt++; rv+=mp->z; }
                mp=g_MatrixMap->PointGetTest(px,py+1); if(mp) { cnt++; rv+=mp->z; }
                mp=g_MatrixMap->PointGetTest(px+1,py+1); if(mp) { cnt++; rv+=mp->z; }
                
                co->m_Matrix._43 = he[i] + rv/cnt;
            } else
            {
                co->m_Matrix._43 = zf[i];
            }

            co->Release();

            o->m_Scale = scale[i];

            o->OnLoad();
            o->Init(type[i]);
            o->m_UID = ouid++;

            if (c7)
            {

                if (c7->GetArrayLength(i) > 0)
                {
                    BYTE *sh = c7->GetFirst<BYTE>(i);

#define GINT() (*(int *)sh); sh += sizeof(int)

                    CMatrixShadowProj *s = HNew(g_MatrixHeap) CMatrixShadowProj(g_MatrixHeap,o);

                    int sz = GINT();

                    for(int g = 0; g < sz; ++g)
                    {
                        int idx = GINT();
                        CMatrixMapGroup *gg = g_MatrixMap->GetGroupByIndex(idx);
                        ASSERT(gg != NULL);
                        gg->AddShadow(s);
                    }

                    sh = s->ReadGeometry(sh);
                    o->m_ShadowProj = s;
                    

                    o->m_ShCampos = *(D3DXVECTOR3 *)sh;
                    o->m_ShDim = *(D3DXVECTOR2 *)(sh+sizeof(D3DXVECTOR3));

                    o->MarkSpecialShadow();

                    o->RNoNeed(MR_ShadowProjGeom);

#undef GINT

                }
            }
        }

        return n;

    }

    if(step == RS_BUILDINGS)
    {
        m_GroundZBaseMiddle = 0;
        m_GroundZBaseMax = 0;

        // loading buildings
        CDataBuf *c0 = stor.GetBuf(DATA_BUILDINGS, DATA_BUILDINGS_X, ST_FLOAT);
        CDataBuf *c1 = stor.GetBuf(DATA_BUILDINGS, DATA_BUILDINGS_Y, ST_FLOAT);
        CDataBuf *c2 = stor.GetBuf(DATA_BUILDINGS, DATA_BUILDINGS_SIDE, ST_BYTE);
        CDataBuf *c3 = stor.GetBuf(DATA_BUILDINGS, DATA_BUILDINGS_KIND, ST_BYTE);
        CDataBuf *c4 = stor.GetBuf(DATA_BUILDINGS, DATA_BUILDINGS_SHADOW, ST_BYTE);
        CDataBuf *c5 = stor.GetBuf(DATA_BUILDINGS, DATA_BUILDINGS_SHADOWSIZE, ST_INT32);
        CDataBuf *c6 = stor.GetBuf(DATA_BUILDINGS, DATA_BUILDINGS_ANGLE, ST_BYTE);


        float *xf = c0->GetFirst<float>(0);
        float *yf = c1->GetFirst<float>(0);
        BYTE *side = c2->GetFirst<BYTE>(0);
        BYTE *kind = c3->GetFirst<BYTE>(0);
        BYTE *shadow = c4->GetFirst<BYTE>(0);
        int *shadowsz = c5->GetFirst<INT32>(0);
        BYTE *ang = c6->GetFirst<BYTE>(0);

        int n = c0->GetArrayLength(0);
        for(int i = 0; i < n; ++i)
        {

            if(side[i] == 255)
            {

                // replace geometry
                int n = (int)kind[i];

                CWStr namet(OBJECT_PATH_BUILDINGS_RUINS, g_MatrixHeap);
                namet += L"b"; namet += n;
                CWStr namev(namet,g_CacheHeap);
                namev += L".vo";

                CMatrixMapObject *mo = g_MatrixMap->StaticAdd<CMatrixMapObject>(false);
                mo->InitAsBaseRuins(D3DXVECTOR2(xf[i],yf[i]),ang[i],namev, namet, true);

                if (n != 0)
                {
                    namev = namet + L"p.vo";
                    namet += L"?Trans";
                    CMatrixMapObject *momo = g_MatrixMap->StaticAdd<CMatrixMapObject>(false);
                    momo->InitAsBaseRuins(D3DXVECTOR2(xf[i],yf[i]),ang[i],namev, namet, false);
                }
            }
            else
            {


                CMatrixBuilding *b;
                b = StaticAdd<CMatrixBuilding>();

                b->m_Pos.x = xf[i];
                b->m_Pos.y = yf[i];
                b->m_Side = side[i];
                b->m_Kind = (EBuildingType)kind[i];
                b->m_ShadowType = (EShadowType)shadow[i];
                b->m_ShadowSize = shadowsz[i];
                b->m_Angle = ang[i];

                b->OnLoad();

                m_GroundZBaseMiddle += b->m_BuildZ;
                if (b->m_BuildZ > m_GroundZBaseMax) m_GroundZBaseMax = b->m_BuildZ;
            }
        }

        m_GroundZBaseMiddle /= float(n);

        return n;
    }


    if(step == RS_ROBOTS)
    {
        // loading robots
        int n = 0;

        CDataBuf *c0 = stor.GetBuf(DATA_ROBOTS, DATA_ROBOTS_X, ST_FLOAT);
        if(c0)
        {
            CDataBuf *c1 = stor.GetBuf(DATA_ROBOTS, DATA_ROBOTS_Y, ST_FLOAT);
            CDataBuf *c2 = stor.GetBuf(DATA_ROBOTS, DATA_ROBOTS_SIDE, ST_BYTE);
            CDataBuf *c3 = stor.GetBuf(DATA_ROBOTS, DATA_ROBOTS_SHADOW, ST_BYTE);
            CDataBuf *c4 = stor.GetBuf(DATA_ROBOTS, DATA_ROBOTS_SHADOWSIZE, ST_INT32);
            CDataBuf *c5 = stor.GetBuf(DATA_ROBOTS, DATA_ROBOTS_GROUP, ST_INT32);
            CDataBuf *c6 = stor.GetBuf(DATA_ROBOTS, DATA_ROBOTS_UNITS, ST_WCHAR);


            float *xf = c0->GetFirst<float>(0);
            float *yf = c1->GetFirst<float>(0);
            BYTE *side = c2->GetFirst<BYTE>(0);
            BYTE *shadow = c3->GetFirst<BYTE>(0);
            int *shadowsz = c4->GetFirst<INT32>(0);
            int *grp = c5->GetFirst<INT32>(0);

            n = c0->GetArrayLength(0);

            for(int i = 0; i < n; ++i)
            {
                SPreRobot sb;
                memset(&sb, 0, sizeof(sb));

                CWStr units(g_CacheHeap);
                units = c6->GetAsWStr(i);

                int cc = units.GetCountPar(L"|");

                int wi = 0;
                for(int k = 0; k < cc; ++k)
                {
                    CWStr unit(units.GetStrPar(k, L"|"), g_CacheHeap);

                    ERobotUnitType type=(ERobotUnitType)unit.GetIntPar(0,L",");
                    ERobotUnitKind kind=(ERobotUnitKind)unit.GetIntPar(1,L",");
                    if(type == MRT_CHASSIS)
                    {
                        sb.sb.m_Chassis.m_nKind = kind;
                        sb.sb.m_Chassis.m_nType = type;

                        sb.angle = (float)unit.GetDoublePar(2,L",");
                    }
                    else if(type == MRT_ARMOR)
                    {
                        sb.sb.m_Armor.m_Unit.m_nKind = kind;
                        sb.sb.m_Armor.m_Unit.m_nType = type;
                    }
                    else if(type == MRT_HEAD)
                    {
                        sb.sb.m_Head.m_nKind = kind;
                        sb.sb.m_Head.m_nType = type;
                    }
                    else if(type == MRT_WEAPON)
                    {
                        sb.sb.m_Weapon[wi].m_Unit.m_nType = type;
                        sb.sb.m_Weapon[wi].m_Unit.m_nKind = kind;
                        ++wi;
                    }
                }

                sb.pos = D3DXVECTOR3(xf[i], yf[i], GetZ(xf[i], yf[i]));
                sb.side = side[i];
                sb.group = grp[i];


                robots->AnyStruct<SPreRobot>(sb);

            }
        }
        return n;
    }

    if (step == RS_CANNONS)
    {

        // loading cannons
        CDataBuf *c0 = stor.GetBuf(DATA_CANNONS, DATA_CANNONS_X, ST_FLOAT);
        CDataBuf *c1 = stor.GetBuf(DATA_CANNONS, DATA_CANNONS_Y, ST_FLOAT);
        CDataBuf *c2 = stor.GetBuf(DATA_CANNONS, DATA_CANNONS_SIDE, ST_BYTE);
        CDataBuf *c3 = stor.GetBuf(DATA_CANNONS, DATA_CANNONS_KIND, ST_BYTE);
        CDataBuf *c4 = stor.GetBuf(DATA_CANNONS, DATA_CANNONS_SHADOW, ST_BYTE);
        CDataBuf *c5 = stor.GetBuf(DATA_CANNONS, DATA_CANNONS_SHADOWSIZE, ST_INT32);
        CDataBuf *c6 = stor.GetBuf(DATA_CANNONS, DATA_CANNONS_ANGLE, ST_FLOAT);
        CDataBuf *c7 = stor.GetBuf(DATA_CANNONS, DATA_CANNONS_PROP, ST_INT32);
        CDataBuf *c8 = stor.GetBuf(DATA_CANNONS, DATA_CANNONS_ADDH, ST_FLOAT);

        float *xf = c0->GetFirst<float>(0);
        float *yf = c1->GetFirst<float>(0);
        BYTE *side = c2->GetFirst<BYTE>(0);
        BYTE *kind = c3->GetFirst<BYTE>(0);
        BYTE *shadow = c4->GetFirst<BYTE>(0);
        int *shadowsz = c5->GetFirst<INT32>(0);
        float *ang = c6->GetFirst<float>(0);
        int *pr = c7?c7->GetFirst<INT32>(0):NULL;
        float *addh = c8?c8->GetFirst<float>(0):NULL;

        int n = c0->GetArrayLength(0);
        for (int i = 0; i < n; ++i)
        {
            CMatrixCannon *c;
            c = StaticAdd<CMatrixCannon>();

skip_this_cannon:

            c->m_ParentBuilding = NULL;

            c->m_Pos.x = xf[i];
            c->m_Pos.y = yf[i];
            c->SetSide(side[i]);
            c->m_Num = (EBuildingType)kind[i];
            c->m_ShadowType = (EShadowType)shadow[i];
            c->m_ShadowSize = shadowsz[i];
            c->m_Angle = ang[i];
            c->m_AddH = addh?addh[i]:0.0f;

            if (pr)
            {
                // Dab!!! 
                int prop = pr[i];
                // 0 - single cannon
                // 1 - factory cannon. must present on map
                // 2 - just place for cannon

                //if(prop==0) c->SetSide(0);

                while(prop==1 || prop==2)
                {
                    // Ищем ближайший завод или базу
                    CMatrixBuilding * building=NULL;
                    CMatrixMapStatic * ms=CMatrixMapStatic::GetFirstLogic();
                    float mindist=1e20f;
                    while(ms) {
                        if(ms->GetObjectType()==OBJECT_TYPE_BUILDING) {
                            CMatrixBuilding * cb=(CMatrixBuilding *)ms;
                            float cdist=POW2(c->m_Pos.x-cb->m_Pos.x)+POW2(c->m_Pos.y-cb->m_Pos.y);
                            if(cb->m_TurretsPlacesCnt<MAX_PLACES && cdist<mindist && cdist<POW2(MAX_DISTANCE_CANNON_BUILDING)) {
                                mindist=cdist;
                                building=cb;
                            }
                        }
                        ms = ms->GetNextLogic();
                    }
                    if(building==NULL)
                    {
                        if(prop==1) prop=2;
                        c->SetSide(0);
                        break;
                    }

                    c->m_ParentBuilding=building;
                    if(prop==1) building->m_TurretsHave++;
                    c->SetSide(building->m_Side);

                    building->m_TurretsPlaces[building->m_TurretsPlacesCnt].m_Coord.x=Float2Int(c->m_Pos.x/GLOBAL_SCALE_MOVE);
                    building->m_TurretsPlaces[building->m_TurretsPlacesCnt].m_Coord.y=Float2Int(c->m_Pos.y/GLOBAL_SCALE_MOVE);
                    building->m_TurretsPlaces[building->m_TurretsPlacesCnt].m_Angle=c->m_Angle;
                    building->m_TurretsPlaces[building->m_TurretsPlacesCnt].m_CannonType = -1;
                    if(prop==1)building->m_TurretsPlaces[building->m_TurretsPlacesCnt].m_CannonType = c->m_Num;
                    //building->m_TurretsPlaces[building->m_TurretsPlacesCnt].m_AddH=c->m_AddH;
                    building->m_TurretsPlacesCnt++;

                    break;
                }

                if (prop == 2)
                {
                    ++i;
                    if (i>=n)
                    {
                        c->m_ParentBuilding= NULL;
                        StaticDelete(c);
                        break;
                    }
                    goto skip_this_cannon;
                }


            } else
            {
                int prop = 0; // single cannon



            }

            c->OnLoad();
        }

        CMatrixMapStatic * ms=CMatrixMapStatic::GetFirstLogic();
        while(ms)
        {
            if(ms->GetObjectType()==OBJECT_TYPE_BUILDING)
            {
                CMatrixBuilding * cb=(CMatrixBuilding *)ms;
                cb->m_TurretsMax=EBuildingTurrets(min(cb->m_TurretsMax,cb->m_TurretsPlacesCnt));
                //if(cb->GetSide() == PLAYER_SIDE && g_MatrixMap->GetPlayerSide()->m_ActiveObject == cb)
                //{
                //    g_IFaceList->CreateDynamicTurrets(cb);
                //}
            }
            ms = ms->GetNextLogic();
        }

        return n;
    }

    if (step == RS_EFFECTS)
    {
        // loading effects

        CDataBuf *efx = stor.GetBuf(DATA_EFFECTS, DATA_EFFECTS_X, ST_FLOAT);
        if (efx != NULL)
        {

            CDataBuf *efy = stor.GetBuf(DATA_EFFECTS, DATA_EFFECTS_Y, ST_FLOAT);
            CDataBuf *efz = stor.GetBuf(DATA_EFFECTS, DATA_EFFECTS_Z, ST_FLOAT);
            CDataBuf *eft = stor.GetBuf(DATA_EFFECTS, DATA_EFFECTS_TYPE, ST_INT32);

            float *xf = efx->GetFirst<float>(0);
            float *yf = efy->GetFirst<float>(0);
            float *zf = efz->GetFirst<float>(0);
            int *ids = eft->GetFirst<int>(0);

            int cnt = efx->GetArrayLength(0);
            for(int i=0;i<cnt;++i)
            {
                AddEffectSpawner(xf[i], yf[i], zf[i], 0, g_MatrixMap->IdsGet(ids[i]));
            }
        }
        return 0;
    }



    if (step == RS_CAMPOS)
    {
        m_CameraAngle = 0;
        ic = propkey->FindAsWStr(DATA_CAMANGLE);
        if (ic >= 0)  m_CameraAngle = (float)propval->GetAsWStr(ic).GetDouble();

        m_Camera.InitStrategyAngle(m_CameraAngle);

        D3DXVECTOR2 cpp;

        ic = propkey->FindAsWStr(DATA_CAMPOSX);
        if(ic >= 0)
        {
            cpp.x = (float)propval->GetAsWStr(ic).GetDouble();
            ic = propkey->FindAsWStr(DATA_CAMPOSY);
            cpp.y = (float)propval->GetAsWStr(ic).GetDouble();

            g_MatrixMap->m_Camera.SetXYStrategy(cpp);
        } 

        {
            // Camera & Select
            ms = CMatrixMapStatic::GetFirstLogic();
            for(; ms; ms = ms->GetNextLogic())
            {
                if(ms->GetObjectType() == OBJECT_TYPE_BUILDING)
                {
                    CMatrixBuilding* bu = (CMatrixBuilding*)ms;
                    if(bu->m_Kind == BUILDING_BASE && bu->GetSide() == PLAYER_SIDE)
                    {
                        float si = TableSin(m_CameraAngle);
                        float co = TableCos(m_CameraAngle);

                        D3DXVECTOR2 bup(bu->m_Pos.x-100*si, bu->m_Pos.y+100*co);

                        if(ic >= 0)
                        {
                            if(D3DXVec2LengthSq(&(bup-cpp)) < POW2(300))
                            {
                                g_MatrixMap->GetPlayerSide()->Select(BUILDING, ms);
                                break;
                            }

                            continue;
                        }

                        g_MatrixMap->GetPlayerSide()->Select(BUILDING, ms);
                        g_MatrixMap->m_Camera.SetXYStrategy(bup);

                        break;
                    }
                }
            }
        }
        g_MatrixMap->m_Camera.Takt(0);
        g_MatrixMap->m_Camera.BeforeDraw();
        
    }

    return 0;
}

DWORD uniq;
CBuf *robots_buf;

int CMatrixMap::PrepareMap(CStorage &stor, const CWStr &mapname)
{
    DTRACE();
    
    robots_buf = NULL;

	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl,sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	mtrl.Specular.r = 0.5f;
	mtrl.Specular.g = 0.5f;
	mtrl.Specular.b = 0.5f;
	mtrl.Specular.a = 0.5f;
	g_D3DD->SetMaterial(&mtrl );
	g_D3DD->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);


    g_LoadProgress->SetCurLP(LP_LOADINGMAP);
    g_LoadProgress->InitCurLP(100000);

    // clear section
    IdsClear();

    // properties

    CDataBuf *propkey = stor.GetBuf(DATA_PROPERTIES, DATA_PROPERTIES_NAME, ST_WCHAR);
    CDataBuf *propval = stor.GetBuf(DATA_PROPERTIES, DATA_PROPERTIES_VALUE, ST_WCHAR);

    if (propkey == NULL) return -1;
    if (propval == NULL) return -1;

    int ic;

    ic = propkey->FindAsWStr(DATA_SIZEX);
    if (ic<0) return -1;
    int sizex = propval->GetAsWStr(ic).GetInt();

    ic = propkey->FindAsWStr(DATA_SIZEY);
    if (ic<0) return -1;
    int sizey = propval->GetAsWStr(ic).GetInt();

    ic = propkey->FindAsWStr(DATA_MACROTEXTURE);
    if (ic >= 0)
    {
        CWStr mt(g_CacheHeap);
        mt = propval->GetAsWStr(ic);
        if (mt.IsEmpty())
        {
            MacrotextureClear();
        } else
        {
            MacrotextureInit(mt);
        }

    } else
    {
        MacrotextureClear();
    }

    g_LoadProgress->SetCurLPPos(100);

    m_BiasCannons = -1.0f;
    m_BiasRobots = -1.0f;
    m_BiasBuildings = -1.0f;
    m_BiasTer = -1.0f;
    m_BiasWater = -1.0f;

    uniq = 0;
    ic = propkey->FindAsWStr(DATA_UNIQID);
    if (ic >= 0)  uniq = propval->GetAsWStr(ic).GetDword();

    m_TexUnionDim = 16;
    m_TexUnionSize = m_TexUnionDim*m_TexUnionDim;

    ic = propkey->FindAsWStr(CWStr(DATA_TEXUNIONDIM));
    if (ic >= 0)
    {
        m_TexUnionDim = propval->GetAsWStr(ic).GetInt();
        m_TexUnionSize = m_TexUnionDim*m_TexUnionDim;
    }

    m_WaterNormalLen = 1;
    ic = propkey->FindAsWStr(DATA_WATERNORMLEN);
    if (ic >= 0)  m_WaterNormalLen = (float)propval->GetAsWStr(ic).GetDouble();

    ic = propkey->FindAsWStr(DATA_BIASTER);
    if (ic >= 0)  m_BiasTer = (float)propval->GetAsWStr(ic).GetDouble();

    ic = propkey->FindAsWStr(DATA_BIASWATER);
    if (ic >= 0)  m_BiasWater = (float)propval->GetAsWStr(ic).GetDouble();

    ic = propkey->FindAsWStr(DATA_BIASCANNONS);
    if (ic >= 0)  m_BiasCannons = (float)propval->GetAsWStr(ic).GetDouble();

    ic = propkey->FindAsWStr(DATA_BIASROBOTS);
    if (ic >= 0)  m_BiasRobots = (float)propval->GetAsWStr(ic).GetDouble();

    ic = propkey->FindAsWStr(DATA_BIASBUILDINGS);
    if (ic >= 0)  m_BiasBuildings = (float)propval->GetAsWStr(ic).GetDouble();

    m_Terrain2ObjectInfluence = 0;
    m_Terrain2ObjectTargetColor = 0;

    ic = propkey->FindAsWStr(DATA_INFLUENCE);
    if (ic >= 0)
    {
        m_Terrain2ObjectInfluence = (float)propval->GetAsWStr(ic).GetDouble();
        if (m_Terrain2ObjectInfluence > 0)
        {
            m_Terrain2ObjectTargetColor = 0xFFFFFFFF;
        } else
        {
            MAKE_ABS_FLOAT(m_Terrain2ObjectInfluence);
            m_Terrain2ObjectTargetColor = 0;
        }
        if (m_Terrain2ObjectInfluence > 1.0) m_Terrain2ObjectInfluence = 1.0f;

    }

    ic = propkey->FindAsWStr(DATA_WATERCOLOR);
    if (ic >= 0)  m_WaterColor = propval->GetAsWStr(ic).GetDword();

    m_SkyColor = DEF_SKY_COLOR | 0xFF000000;
    ic = propkey->FindAsWStr(DATA_SKYCOLOR);
    if (ic >= 0)  m_SkyColor = propval->GetAsWStr(ic).GetDword()  | 0xFF000000;

    ic = propkey->FindAsWStr(DATA_SKYNAME);
    if (ic >= 0)
    {
        CWStr skyname(g_CacheHeap);
        skyname = propval->GetAsWStr(ic);

        CBlockPar *skbp = g_MatrixData->BlockGet(L"Sky")->BlockGet(skyname);

        m_SkyAngle = GRAD2RAD((float)skbp->ParGet(L"Angle").GetDouble());
        m_SkyDeltaAngle = GRAD2RAD((float)skbp->ParGet(L"DeltaAngle").GetDouble());
    
        m_Reflection = (CTextureManaged *)g_Cache->Get(cc_TextureManaged,skbp->ParGet(L"Reflection"));

        if (g_Config.m_SkyBox != 0)
        {
            for (int idx=0;idx <4; ++idx)
            {

                if (idx == 0) skyname = L"Fore";
                else if (idx == 1) skyname = L"Back";
                else if (idx == 2) skyname = L"Left";
                else if (idx == 3) skyname = L"Right";
                //else if (idx == 4) skyname = L"Up";

                CWStr texname(skbp->ParGet(skyname).GetStrPar(0,L","), g_CacheHeap);

                if (g_Config.m_SkyBox == 2) texname += L"_high";


                m_SkyTex[idx].tex = (CTextureManaged *)g_Cache->Get(cc_TextureManaged,texname.Get());
                CTextureManaged *tex = m_SkyTex[idx].tex;
                tex->MipmapOff();
                tex->Prepare();
                
                m_SkyTex[idx].u0 = float(skbp->ParGet(skyname).GetIntPar(1,L",")) / float(tex->GetSizeX());
                m_SkyTex[idx].v0 = float(skbp->ParGet(skyname).GetIntPar(2,L",")) / float(tex->GetSizeY());
                m_SkyTex[idx].u1 = float(skbp->ParGet(skyname).GetIntPar(3,L",")) / float(tex->GetSizeX());
                m_SkyTex[idx].v1 = float(skbp->ParGet(skyname).GetIntPar(4,L",")) / float(tex->GetSizeY());


            }
        }
    }

    ic = propkey->FindAsWStr(DATA_SKYANGLE);
    if (ic >= 0)
    {
        m_SkyAngle += (float)propval->GetAsWStr(ic).GetDouble();
    }

    if (m_Reflection == NULL)
    {
        m_Reflection = (CTextureManaged *)g_Cache->Get(cc_TextureManaged,TEXTURE_PATH_REFLECTION);
    }
    m_Reflection->Preload();


    ic = propkey->FindAsWStr(DATA_INSHOREWAVECOLOR);
    if (ic >= 0)  m_InshorewaveColor = propval->GetAsWStr(ic).GetDword();

    ic = propkey->FindAsWStr(DATA_AMBIENTCOLOROBJ);
    if (ic >= 0)  m_AmbientColorObj = propval->GetAsWStr(ic).GetDword();

    ic = propkey->FindAsWStr(DATA_AMBIENTCOLOR);
    if (ic >= 0)  m_AmbientColor = propval->GetAsWStr(ic).GetDword();

    ic = propkey->FindAsWStr(DATA_LIGHTMAINCOLOR);
    if (ic >= 0)  m_LightMainColor = propval->GetAsWStr(ic).GetDword();

    ic = propkey->FindAsWStr(DATA_LIGHTMAINCOLOROBJ);
    if (ic >= 0)  m_LightMainColorObj = propval->GetAsWStr(ic).GetDword();

    ic = propkey->FindAsWStr(DATA_LIGHTMAINANGLEX);
    if (ic >= 0)  m_LightMainAngleX = (float)propval->GetAsWStr(ic).GetDouble();

    ic = propkey->FindAsWStr(DATA_LIGHTMAINANGLEZ);
    if (ic >= 0)  m_LightMainAngleZ = (float)propval->GetAsWStr(ic).GetDouble();

    ic = propkey->FindAsWStr(DATA_SHADOWCOLOR);
    if (ic >= 0)  m_ShadowColor = propval->GetAsWStr(ic).GetDword();


    m_WaterName = L"water_blue";
    ic = propkey->FindAsWStr(DATA_WATERNAME);
    if (ic >= 0)
    {
        m_WaterName = propval->GetAsWStr(ic);
    }
    if (g_MatrixData->BlockCount(PAR_SOURCE_WATER) > 0)
    {
        if (g_MatrixData->BlockGet(PAR_SOURCE_WATER)->BlockCount(m_WaterName) == 0)
        {
            m_WaterName = g_MatrixData->BlockGet(PAR_SOURCE_WATER)->BlockGetName(0);
        }
    }

    ReloadDynamics(stor, RS_SIDEAI);
    ReloadDynamics(stor, RS_RESOURCES);

    g_LoadProgress->SetCurLPPos(200);

	D3DXMATRIX m1,m2,m3;
	D3DXMatrixRotationX(&m1,m_LightMainAngleX);
	D3DXMatrixRotationZ(&m2,m_LightMainAngleZ);
	m3=m1*m2;
	D3DXVec3TransformNormal(&m_LightMain,&D3DXVECTOR3(0.0f,0.0f,-1.0f),&m3);

    UnitInit(sizex,sizey);

    // loading points and units
    CDataBuf *ptsc = stor.GetBuf(DATA_POINTS, DATA_POINTS_DATA, ST_BYTE);
    SMatrixMapUnit *mu = m_Unit;
    SMatrixMapPoint *mp = m_Point;
    SCompilePoint *cp = ptsc->GetFirst<SCompilePoint>(0);

    CDataBuf *movc = stor.GetBuf(DATA_MOVE, DATA_MOVE_DATA, ST_BYTE);
    SCompileMoveCell *cc = movc->GetFirst<SCompileMoveCell>(0);

    SMatrixMapMove *smm = m_Move;

    g_LoadProgress->SetCurLPPos(300);

    for (int y = 0; y<(m_Size.y+1); ++y, smm += m_SizeMove.x)
    {
        for (int x = 0; x<(m_Size.x+1); ++x, ++mp, ++cp)
        {
            mp->color = (cp->r << 16) | (cp->g << 8) | (cp->b << 0);
            mp->z = cp->z;
            mp->z_land = max(0,cp->z);

            if (mp->z < m_minz) m_minz = mp->z;
            if (mp->z > m_maxz) m_maxz = mp->z;

            mp->lum_r = 0; mp->lum_g = 0; mp->lum_b = 0;
            if (x < m_Size.x && y < m_Size.y)
            {
                SCompileMoveCell *cur = cc + cp->move;

                smm->m_Zone = cur->c[0].m_Zone;
                smm->m_Zubchik = cur->c[0].m_Zubchik;
                smm->m_Sphere = cur->c[0].m_Sphere;
                smm->m_Stop = cur->c[0].m_Move;

                (smm+1)->m_Zone = cur->c[1].m_Zone;
                (smm+1)->m_Zubchik = cur->c[1].m_Zubchik;
                (smm+1)->m_Sphere = cur->c[1].m_Sphere;
                (smm+1)->m_Stop = cur->c[1].m_Move;

                (smm+m_SizeMove.x)->m_Zone = cur->c[2].m_Zone;
                (smm+m_SizeMove.x)->m_Zubchik = cur->c[2].m_Zubchik;
                (smm+m_SizeMove.x)->m_Sphere = cur->c[2].m_Sphere;
                (smm+m_SizeMove.x)->m_Stop = cur->c[2].m_Move;

                (smm+m_SizeMove.x+1)->m_Zone = cur->c[3].m_Zone;
                (smm+m_SizeMove.x+1)->m_Zubchik = cur->c[3].m_Zubchik;
                (smm+m_SizeMove.x+1)->m_Sphere = cur->c[3].m_Sphere;
                (smm+m_SizeMove.x+1)->m_Stop = cur->c[3].m_Move;

                mu->SetType(cp->flags);
                ++mu;
                smm += MOVE_CNT;
            }
        }

    }

    g_LoadProgress->SetCurLPPos(1000);

    // calc normals
	for(int y=0;y<=m_Size.y;y++)
    {
		for(int x=0;x<=m_Size.x;x++)
        {
			PointCalcNormals(x,y);
		}
	}

    g_LoadProgress->SetCurLPPos(1500);

    // IDS Loading...

    {
        CDataBuf *strc = stor.GetBuf(DATA_STRINGS, DATA_STRINGS_STRING, ST_WCHAR);
	    m_IdsCnt=strc->GetArraysCount() + 1;
	    m_Ids=(CWStr *)HAlloc(m_IdsCnt*sizeof(CWStr),g_MatrixHeap);
        int cnt = (m_IdsCnt-1);
	    for(int i=0;i<cnt;i++)
        {
            m_Ids[i].CWStr::CWStr(strc->GetAsWStr(i), g_MatrixHeap);
        }
        m_Ids[cnt].CWStr::CWStr(mapname,g_MatrixHeap);

    }

    g_LoadProgress->SetCurLPPos(2000);

    BuildTexUnions(stor, 2100, 50000);
    GroupBuild(stor);

    g_LoadProgress->SetCurLPPos(51000);

    // prepare units coefs

    mu = m_Unit;
    mp = m_Point;
    for (int y=0; y<m_Size.y; ++y)
    {
        for (int x=0; x<m_Size.x; ++x, ++mu, ++mp)
        {
            if (mu->IsWater()) continue;

            D3DXVECTOR3 p0,p1,p2,p3;
	        D3DXPLANE pl;
            float cc;

	        p0.x=0;
	        p0.y=0;
	        p0.z=mp->z;
	        p1.x=GLOBAL_SCALE;
	        p1.y=0;
	        p1.z=(mp+1)->z;
	        p2.x=GLOBAL_SCALE;
	        p2.y=GLOBAL_SCALE;
	        p2.z=(mp+m_Size.x+2)->z;
	        p3.x=0;
	        p3.y=GLOBAL_SCALE;
	        p3.z=(mp+m_Size.x+1)->z;

            if (mu->IsFlat()) 
            {
                mu->a1 = p0.z;
            } else
            {

	            D3DXPlaneFromPoints(&pl,&p0,&p1,&p2);
                cc = -1.0f / pl.c;
                mu->a1 = pl.a * cc;
                mu->b1 = pl.b * cc;
                mu->c1 = pl.d * cc;
    	        
                D3DXPlaneFromPoints(&pl,&p0,&p2,&p3);
                cc = -1.0f / pl.c;
                mu->a2 = pl.a * cc;
                mu->b2 = pl.b * cc;
                mu->c2 = pl.d * cc;
            }

        }
        ++mp;
    }

    g_LoadProgress->SetCurLPPos(52000);
    
    ic = propkey->FindAsWStr(DATA_DISABLEINSHORE);
    if (ic >= 0)
    {
        INITFLAG(m_Flags, MMFLAG_DISABLEINSHORE_BUILD, propval->GetAsWStr(ic).GetInt() != 0);
    }

    // building water
    CDataBuf *inshore_x = stor.GetBuf(DATA_GROUPS_INSHORES, DATA_GROUPS_INSHORES_X, ST_FLOAT);
    if (inshore_x != NULL && !FLAG(m_Flags, MMFLAG_DISABLEINSHORE_BUILD))
    {
        // loading inshores

        CDataBuf *inshore_y = stor.GetBuf(DATA_GROUPS_INSHORES, DATA_GROUPS_INSHORES_Y, ST_FLOAT);
        CDataBuf *inshore_nx = stor.GetBuf(DATA_GROUPS_INSHORES, DATA_GROUPS_INSHORES_NX, ST_FLOAT);
        CDataBuf *inshore_ny = stor.GetBuf(DATA_GROUPS_INSHORES, DATA_GROUPS_INSHORES_NY, ST_FLOAT);

        int index = 0;

        for (int j=0;j<m_GroupSize.y;++j)
        {
            for (int i=0;i<m_GroupSize.x;++i,++index)
            {
                int n = inshore_x->GetArrayLength(index);
                if (n == 0) continue;

                float *xx = inshore_x->GetFirst<float>(index);
                float *yy = inshore_y->GetFirst<float>(index);
                float *nxx = inshore_nx->GetFirst<float>(index);
                float *nyy = inshore_ny->GetFirst<float>(index);

                PCMatrixMapGroup g = GetGroupByIndex(i,j);
                if (g)
                    g->InitInshoreWaves(n, xx, yy, nxx, nyy);

                
            }
        }

        SETFLAG(m_Flags, MMFLAG_DISABLEINSHORE_BUILD);
    }


    {
        int x = 0;
        int y = 0;
        int cntg = m_GroupSize.x * m_GroupSize.y;

        float deltalp = 30000.0f / float(cntg);
        float clp = 52100.0f;


        for (int i=0;i<cntg;++i,++x)
        {
            g_LoadProgress->SetCurLPPos(Float2Int(clp));
            clp += deltalp;


            if (x >= m_GroupSize.x)
            {
                x = 0;
                ++y;
            }
            if (m_Group[i]) m_Group[i]->BuildWater(x,y);
        }
    }

    // load bridges here, to set z of points

    {
        CDataBuf *br = stor.GetBuf(DATA_BRIDGES, DATA_BRIDGES_DATA, ST_BYTE);
        if (br)
        {

		    for(DWORD i=0;i<br->GetArraysCount();++i)
            {
                const CRect *most = br->GetFirst<CRect>(i);

                int dx = (most->right-most->left);
                //int cntz = dx*(most->bottom-most->top);
                float *z = (float *)(most + 1);

                SMatrixMapUnit *cu = UnitGet(most->left, most->top);
                SMatrixMapPoint *cp = PointGet(most->left, most->top);
                int dcu = m_Size.x - dx + 1;
                int dcp = m_Size.x - dx + 1;

                D3DXVECTOR3 p0,p1,p2,p3;
	            p0.x=0;
	            p0.y=0;
	            p1.x=GLOBAL_SCALE;
	            p1.y=0;
	            p2.x=GLOBAL_SCALE;
	            p2.y=GLOBAL_SCALE;
	            p3.x=0;
	            p3.y=GLOBAL_SCALE;

                for (int y=most->top; y<most->bottom; ++y, cu += dcu, cp += dcp)
                {
                    for (int x=most->left; x<most->right; ++x, ++cp, ++z)
                    {

                        p0.z = *z;

                        if ((x < (most->right-1)) && (y < (most->bottom-1)))
                        {
                            cu->SetBridge();

                            p1.z = *(z+1);
                            p2.z = *(z + dx + 1);
                            p3.z = *(z + dx);

                            cu->ResetFlat();
                            if ((p0.z == p1.z) && (p0.z == p2.z) && (p0.z == p3.z)) 
                            {
                                cu->SetFlat();
                                cu->a1 = p0.z;
                            }
                            else
                            {
	                            D3DXPLANE pl;
                                float cc;


	                            D3DXPlaneFromPoints(&pl,&p0,&p1,&p2);
                                cc = -1.0f / pl.c;
                                cu->a1 = pl.a * cc;
                                cu->b1 = pl.b * cc;
                                cu->c1 = pl.d * cc;
                	         
                                D3DXPlaneFromPoints(&pl,&p0,&p2,&p3);
                                cc = -1.0f / pl.c;
                                cu->a2 = pl.a * cc;
                                cu->b2 = pl.b * cc;
                                cu->c2 = pl.d * cc;
                            }

                            ++cu;
                        }

                        cp->z = p0.z;

                        
                        //CHelper::Create(100000,0)->Line(D3DXVECTOR3((x)*GLOBAL_SCALE, (y)*GLOBAL_SCALE, 50), 
                        //                             D3DXVECTOR3((x)*GLOBAL_SCALE, (y)*GLOBAL_SCALE, 50 + 10 * PointGet(x,y)->z));

                        //D3DXVECTOR3 p(x * GLOBAL_SCALE, y * GLOBAL_SCALE, PointGet(x,y)->z);
                        ////D3DXVECTOR3 p(x * GLOBAL_SCALE, y * GLOBAL_SCALE, 300);

                        //CHelper::Create(100000,0)->Cone(p,p+D3DXVECTOR3(0,0,10), 5,5,0xFFFFFF00,0xFF00FFFF,7);
                    }
                }
            }
        }
    }

    g_LoadProgress->SetCurLPPos(82000);

    // loading surfaces
    //if (0)
    {

        bool striped = false;
        ic = propkey->FindAsWStr(DATA_TOPTEXSTRIPED);
        if (ic >= 0)  striped = propval->GetAsWStr(ic) == L"1";

        CDataBuf *srfc = stor.GetBuf(DATA_SURFACES, DATA_SURFACES_DATA, ST_BYTE);
        CDataBuf *srfcm = stor.GetBuf(DATA_SURFACES_M, DATA_SURFACES_DATA_M, ST_BYTE);

        int whole = srfcm->GetArraysCount() + srfc->GetArraysCount();

        if (whole > 0 && !striped)
        {
            ERROR_S(L"Sorry, top textures are not stripified. Recompile map with last editor...");
        }

        CTerSurface::AllocSurfaces(whole);



        int index = 0;

        float deltalp = 5000.0f / float(srfcm->GetArraysCount() + srfc->GetArraysCount());
        float clp = 82100.0f;

        for (DWORD i=0; i<srfcm->GetArraysCount(); ++i)
        {
            g_LoadProgress->SetCurLPPos(Float2Int(clp));
            clp += deltalp;

            CTerSurface::LoadM(i + index, srfcm->GetFirst<BYTE>(i));
        }

        index = srfcm->GetArraysCount();
        for (DWORD i=0; i<srfc->GetArraysCount(); ++i)
        {
            g_LoadProgress->SetCurLPPos(Float2Int(clp));
            clp += deltalp;

            CTerSurface::Load(i + index, srfc->GetFirst<BYTE>(i));
        }

    }


    g_LoadProgress->SetCurLPPos(88000);
    int allobj = ReloadDynamics(stor, RS_MAPOBJECTS);
    g_LoadProgress->SetCurLPPos(89000);
    allobj += ReloadDynamics(stor,RS_BUILDINGS);
    g_LoadProgress->SetCurLPPos(89500);

    ASSERT(robots_buf == NULL);
    robots_buf = HNew(g_CacheHeap) CBuf(g_CacheHeap);

    allobj += ReloadDynamics(stor, RS_ROBOTS, robots_buf);
    g_LoadProgress->SetCurLPPos(90000);
    allobj += ReloadDynamics(stor, RS_CANNONS);
    g_LoadProgress->SetCurLPPos(91000);
    ReloadDynamics(stor, RS_EFFECTS);
    g_LoadProgress->SetCurLPPos(92000);

    CDataBuf *roads = stor.GetBuf(DATA_ROADS, DATA_ROADS_DATA, ST_BYTE);
    if (roads && roads->GetArrayLength(0) > 0)
    {
        CBuf rnb(g_CacheHeap);
        rnb.BufAdd(roads->GetFirst<BYTE>(0), roads->GetArrayLength(0));
        rnb.Pointer(0);
        DWORD ver = rnb.Dword();
        if(ver!=27) ERROR_S(L"Please, recompile map with last editor...");

        m_RN.Load(rnb,ver);
        m_RN.InitPL(m_SizeMove.x,m_SizeMove.y);
    }


    // prepare vis
    CDataBuf *dbl = stor.GetBuf(DATA_GROUPS_VIS, DATA_GROUPS_VIS_LEVELS, ST_INT32);

    if (dbl)
    {
        CDataBuf *dbg = stor.GetBuf(DATA_GROUPS_VIS, DATA_GROUPS_VIS_GROUPS, ST_INT32);
        CDataBuf *dbz = stor.GetBuf(DATA_GROUPS_VIS, DATA_GROUPS_VIS_ZFROM, ST_FLOAT);

        float *z = dbz->GetFirst<float>(0);

        int gcnt = m_GroupSize.x * m_GroupSize.y;

        m_GroupVis = (SGroupVisibility *)HAllocClear(sizeof(SGroupVisibility) * gcnt, g_MatrixHeap);
        ASSERT(gcnt == dbl->GetArraysCount());

        for (int i=0;i<gcnt;++i)
        {
            SGroupVisibility *gv = m_GroupVis + i;

            gv->levels_cnt = dbl->GetArrayLength(i);
            gv->levels = (int *)HAlloc(gv->levels_cnt * sizeof(int), g_MatrixHeap);
            memcpy(gv->levels, dbl->GetFirst<int>(i), gv->levels_cnt * sizeof(int));

            gv->vis_cnt = dbg->GetArrayLength(i);
            gv->vis = (PCMatrixMapGroup *)HAlloc(gv->vis_cnt * sizeof(PCMatrixMapGroup), g_MatrixHeap);
            int *f = dbg->GetFirst<int>(i);
            for (int t=0;t<gv->vis_cnt;++t)
            {
                gv->vis[t] = g_MatrixMap->m_Group[f[t]];
            }
            gv->z_from = z[i];


        }

    }



    g_LoadProgress->SetCurLPPos(94000);

 
    StaticPrepare(allobj);
    m_Cursor.SetPos(100,100);


    m_StartTime = timeGetTime();
    for (int i=0;i<m_SideCnt;++i)
    {
        //m_Side[i].SetStatus(SS_NONE);
        m_Side[i].ClearStatistics();
    }

    return ReloadDynamics(stor, RS_CAMPOS);
}

void CMatrixMap::StaticPrepare2(CBuf *robots)
{
    // prepare shadows textures
    CMatrixMapStatic **sb = m_AllObjects.Buff<CMatrixMapStatic *>();
    CMatrixMapStatic **se = m_AllObjects.BuffEnd<CMatrixMapStatic *>();
	while(sb < se)
    {
        if ((*sb)->GetObjectType() == OBJECT_TYPE_MAPOBJECT)
        {
            if (((CMatrixMapObject *)(*sb))->m_ShadowType == SHADOW_PROJ_DYNAMIC && ((CMatrixMapObject *)(*sb))->m_Graph->VO()->GetFramesCnt() > 1)
            {

            }
            else
            {
                (*sb)->RNeed(MR_ShadowProjTex);
            }
        } else
        {
            (*sb)->RNeed(MR_ShadowProjTex);

            if ((*sb)->IsBase() && (*sb)->IsLiveBuilding())
            {
                int side = (*sb)->GetSide();
                if (side != 0)
                {
                    g_MatrixMap->GetSideById(side)->SetStatus(SS_ACTIVE);
                }
            }
        }
		++sb;
	}

    if (robots)
    {
        SPreRobot *pr0 = robots->Buff<SPreRobot>();
        SPreRobot *pr1 = robots->BuffEnd<SPreRobot>();
        for (; pr0<pr1; ++pr0)
        {
            CMatrixRobotAI * r = pr0->sb.GetRobot(pr0->pos, pr0->side);
            r->m_Forward.x = float(-sin(pr0->angle));
            r->m_Forward.y = float(cos(pr0->angle));
            g_MatrixMap->AddObject(r,true);
            r->JoinToGroup();
            r->CreateTextures();
            
            int side = r->GetSide();
            if (side != 0)
            {
                g_MatrixMap->GetSideById(side)->SetStatus(SS_ACTIVE);
            }

            r->MapPosCalc();

            // use a group: pr0->group
            if(side!=PLAYER_SIDE) {
                if(pr0->group>=1 && pr0->group<=3) r->SetTeam(pr0->group-1);
                else r->SetTeam(-1);
            } else {
                SETFLAG(m_Flags, MMFLAG_SOUND_ORDER_ATTACK_DISABLE);
                //g_MatrixMap->GetSideById(side)->PGOrderAttack(g_MatrixMap->GetSideById(side)->RobotToLogicGroup(r),CPoint(r->GetMapPosX(),r->GetMapPosY()),NULL);
                g_MatrixMap->GetSideById(side)->PGOrderStop(g_MatrixMap->GetSideById(side)->RobotToLogicGroup(r));
                RESETFLAG(m_Flags, MMFLAG_SOUND_ORDER_ATTACK_DISABLE);

            }
        }
        for(int i=0;i<m_SideCnt;i++) m_Side[i].GroupNoTeamRobot();
    }
}

void CMatrixMap::StaticPrepare(int ocnt, bool skip_progress)
{
    DTRACE();

    int n = 0;
    if (!skip_progress)
    {
        g_LoadProgress->SetCurLP(LP_PREPARINGOBJECTS);
        g_LoadProgress->InitCurLP(ocnt * 2);
    }

	CMatrixMapStatic **sb = m_AllObjects.Buff<CMatrixMapStatic *>();
    CMatrixMapStatic **se = m_AllObjects.BuffEnd<CMatrixMapStatic *>();

	while(sb < se)
    {
        (*sb)->RNeed(MR_Matrix|MR_Graph);
        if ((*sb)->GetObjectType() != OBJECT_TYPE_MAPOBJECT)
        {
            (*sb)->JoinToGroup();
//b0
//x -8,896
//y 68,774
//z 55,495
//
//b1 titan
//x 0,455
//y 58,688
//z 73,993
//
//b2 plasma
//x -0,149
//y 69,396
//z 72,981
//
//b3 electro
//x 0,124
//y 64,68
//z 75,081
//
//b4 energy
//x 0,1
//y 72,302
//z 51,103

            if ((*sb)->IsBuilding())
            {
                SObjectCore* core = (*sb)->GetCore(DEBUG_CALL_INFO);
                if((*sb)->AsBuilding()->m_Kind == BUILDING_BASE){
                    D3DXVec3TransformCoord(&(*sb)->AsBuilding()->m_TopPoint, &D3DXVECTOR3(-8.896f, -68.774f, 55.495f), &core->m_Matrix);
                }else if((*sb)->AsBuilding()->m_Kind == BUILDING_TITAN){
                    D3DXVec3TransformCoord(&(*sb)->AsBuilding()->m_TopPoint, &D3DXVECTOR3(0.455f, 58.688f, 73.993f), &core->m_Matrix);
                }else if((*sb)->AsBuilding()->m_Kind == BUILDING_ELECTRONIC){
                    D3DXVec3TransformCoord(&(*sb)->AsBuilding()->m_TopPoint, &D3DXVECTOR3(0.124f, 64.68f, 75.081f), &core->m_Matrix);
                }else if((*sb)->AsBuilding()->m_Kind == BUILDING_ENERGY){
                    D3DXVec3TransformCoord(&(*sb)->AsBuilding()->m_TopPoint, &D3DXVECTOR3(0.1f, 110.0f, 51.103f), &core->m_Matrix);
                }else if((*sb)->AsBuilding()->m_Kind == BUILDING_PLASMA){
                    D3DXVec3TransformCoord(&(*sb)->AsBuilding()->m_TopPoint, &D3DXVECTOR3(-0.149f, 105.0f, 72.981f), &core->m_Matrix);
                }
                
                core->Release();
            }
        }
		++sb;
        ++n;
	}

    

    // prepare shadows geometry
    sb = m_AllObjects.Buff<CMatrixMapStatic *>();
	while(sb < se)
    {
        (*sb)->RNeed(MR_ShadowProjGeom);
        //ms->FreeDynamicResources();
		++sb;
        if (!skip_progress)
        {
            g_LoadProgress->SetCurLPPos(n++);
        }
	}
}


void CMatrixMap::Restart(void)
{

	m_BeforeWinCount = 0;
    m_BeforeWinLooseDialogCount = 0;
    if (SETFLAG(m_Flags, MMFLAG_WIN))
        RESETFLAG(m_Flags, MMFLAG_WIN);

    CDWORDMap dm(g_CacheHeap);

    CMatrixMapStatic ** sb = m_AllObjects.Buff<CMatrixMapStatic *>();
    CMatrixMapStatic ** se = m_AllObjects.BuffEnd<CMatrixMapStatic *>();
    for (;sb < se;++sb)
    {
        if ((*sb)->GetObjectType() == OBJECT_TYPE_MAPOBJECT)
        {
            if ((*sb)->IsNotOnMinimap())
            {
                int uid = ((CMatrixMapObject *)(*sb))->m_UID;
                if (uid >= 0) dm.Set(uid,0);
            }
        }
    }
    

    // removing all objects
    StaticClear();

    if (m_EffectSpawners)
    {
        for (int i = 0; i<m_EffectSpawnersCnt; ++i)
        {
            m_EffectSpawners[i].~CEffectSpawner();
        }
        HFree(m_EffectSpawners, g_MatrixHeap);
    }
    m_EffectSpawnersCnt = 0;
    m_EffectSpawners = NULL;

    // removing all effects
    while (m_EffectsFirst)
    {
#ifdef _DEBUG
        SubEffect(DEBUG_CALL_INFO, m_EffectsFirst);
#else
        SubEffect(m_EffectsFirst);
#endif
    }

    //CMatrixEffectPointLight::ClearAll();

    m_StartTime = timeGetTime();
    for (int i=0;i<m_SideCnt;++i)
    {
        m_Side[i].SetStatus(SS_NONE);
        m_Side[i].ClearStatistics();
    }

#ifdef _DEBUG
    CMatrixMapObject::ValidateAfterReset();
    CMatrixMapStatic::ValidateAfterReset();
#endif



    CStorage stor(g_CacheHeap);

    stor.Load(MapName());

    ReloadDynamics(stor, RS_SIDEAI);
    ReloadDynamics(stor, RS_RESOURCES);
    int allobj = ReloadDynamics(stor,RS_MAPOBJECTS);    // objects
    allobj += ReloadDynamics(stor,RS_BUILDINGS);       // buildings
    CBuf robots(g_CacheHeap);
    allobj += ReloadDynamics(stor, RS_ROBOTS,&robots);
    allobj += ReloadDynamics(stor, RS_CANNONS);

    ReloadDynamics(stor, RS_EFFECTS);

    StaticPrepare(allobj, true);
    StaticPrepare2(&robots);

    ReloadDynamics(stor, RS_CAMPOS);

    // minimap rendering
    {

	    CMatrixMapStatic **sb = m_AllObjects.Buff<CMatrixMapStatic *>();
        CMatrixMapStatic **se = m_AllObjects.BuffEnd<CMatrixMapStatic *>();

        DWORD flags = 0;

        if (g_Config.m_DrawAllObjectsToMinimap == 1) flags = MR_Matrix|MR_Graph|MR_MiniMap;
        else if (g_Config.m_DrawAllObjectsToMinimap == 2)
        {

            flags = MR_Matrix|MR_Graph|MR_MiniMap;
        }

        if (flags != 0)
        {
	        while(sb < se)
            {
                int uid = ((CMatrixMapObject *)(*sb))->m_UID;
                if (dm.Get(uid,NULL))
                {
                    dm.Del(uid);
                    flags &= ~MR_MiniMap;
                    (*sb)->RNoNeed(MR_MiniMap);
                }

                (*sb)->RNeed(flags);
		        ++sb;
	        }
        }
    }

    CMatrixMapStatic *ms = CMatrixMapStatic::GetFirstLogic();
    while(ms!=NULL)
    {
        if (ms->IsLiveBuilding())
        {
            ((CMatrixBuilding *)ms)->LogicTakt(100000);
            ms->RNeed(MR_Matrix|MR_Graph|MR_MiniMap);
            //RenderBuildingToMiniMapBackground((CMatrixBuilding *)ms);
        }
        ms = ms->GetNextLogic();
    }

    g_MatrixMap->CalcCannonPlace();

    RESETFLAG(g_MatrixMap->m_Flags, MMFLAG_SOUND_BASE_SEL_ENABLED);

    g_MatrixMap->EnterDialogMode(TEMPLATE_DIALOG_BEGIN);

}

void CMatrixMap::InitObjectsLights(void)
{
    // init lights
	CMatrixMapStatic **sb = m_AllObjects.Buff<CMatrixMapStatic *>();
    CMatrixMapStatic **se = m_AllObjects.BuffEnd<CMatrixMapStatic *>();
	for(;sb < se;++sb)
    {
        if ((*sb)->GetObjectType() == OBJECT_TYPE_MAPOBJECT)
        {
            ((CMatrixMapObject *)(*sb))->m_Graph->InitLights(CMatrixEffect::GetBBTexI(BBT_POINTLIGHT));
        }
    }

}

void    CMatrixMap::CreatePoolDefaultResources(bool loading)
{
    // side color textures
    DTRACE();

    for (int i=0;i<m_SideCnt;++i)
    {
        if (m_Side[i].m_ColorTexture == NULL)
        {
            m_Side[i].m_ColorTexture = CACHE_CREATE_TEXTURE();
            m_Side[i].m_ColorTexture->MakeSolidTexture(2,2,m_Side[i].m_Color);
        }
    }
    if (m_NeutralSideColorTexture == NULL)
    {
        m_NeutralSideColorTexture = CACHE_CREATE_TEXTURE();
        m_NeutralSideColorTexture->MakeSolidTexture(2,2,m_NeutralSideColor);
    }



    // init shadow vb

    CMatrixEffect::CreatePoolDefaultResources();

    WaterInit();       // water


  	SShadowRectVertex *v;
    if (IS_VB(m_ShadowVB)) DESTROY_VB(m_ShadowVB);
    CREATE_VB(sizeof(SShadowRectVertex)*4, D3DFVF_XYZRHW|D3DFVF_DIFFUSE, m_ShadowVB);
    LOCK_VB(m_ShadowVB,&v);
	
    v[0].p = D3DXVECTOR4(  0, float(g_ScreenY), 0.0f, 1.0f );
	v[0].color = m_ShadowColor;
	v[1].p = D3DXVECTOR4(  0,  0, 0.0f, 1.0f );
	v[1].color = m_ShadowColor;
	v[2].p = D3DXVECTOR4( float(g_ScreenX), float(g_ScreenY), 0.0f, 1.0f );
	v[2].color = m_ShadowColor;
	v[3].p = D3DXVECTOR4( float(g_ScreenX),  0, 0.0f, 1.0f );
	v[3].color = m_ShadowColor;

    UNLOCK_VB(m_ShadowVB);


    if (loading)
    {
        StaticPrepare2(robots_buf);

        HDelete(CBuf, robots_buf, g_CacheHeap);
        robots_buf = NULL;

        m_Minimap.Init();
        CWStr nnn(MapName(),g_CacheHeap);
        int iii = max(nnn.FindR('\\'), nnn.FindR('/')) ;
        if (iii >=0) nnn.Del(0,iii+1);

        m_Minimap.RenderBackground(nnn, uniq);

        {

	        CMatrixMapStatic **sb = m_AllObjects.Buff<CMatrixMapStatic *>();
            CMatrixMapStatic **se = m_AllObjects.BuffEnd<CMatrixMapStatic *>();

            DWORD flags = 0;

            if (g_Config.m_DrawAllObjectsToMinimap == 1) flags = MR_Matrix|MR_Graph|MR_MiniMap;
            else if (g_Config.m_DrawAllObjectsToMinimap == 2)
            {

                flags = MR_Matrix|MR_Graph|MR_MiniMap;
            }

            if (flags != 0)
            {
	            while(sb < se)
                {
                    (*sb)->RNeed(flags);
		            ++sb;
	            }
            }
        }

        CMatrixMapStatic *ms = CMatrixMapStatic::GetFirstLogic();
        while(ms!=NULL)
        {
            if (ms->IsLiveBuilding())
            {
                ((CMatrixBuilding *)ms)->LogicTakt(100000);
                ms->RNeed(MR_Matrix|MR_Graph|MR_MiniMap);
                //RenderBuildingToMiniMapBackground((CMatrixBuilding *)ms);
            }
            ms = ms->GetNextLogic();
        }
    } else
    {
        m_Minimap.RestoreTexture();
        m_DI.OnResetDevice();
        CBaseTexture::OnResetDevice();
    }



    SETFLAG(m_Flags, MMFLAG_VIDEO_RESOURCES_READY);

    g_Config.ApplyGammaRamp();

    if (m_DeviceState)
    {
        m_DeviceState->RestoreState();
        HDelete(CDeviceState, m_DeviceState, g_MatrixHeap);
        m_DeviceState = NULL;
    }
}

void    CMatrixMap::ReleasePoolDefaultResources(void)
{
    DTRACE();

    if (g_D3DD == NULL) return;

    if (m_DeviceState == NULL)
        m_DeviceState = HNew(g_MatrixHeap) CDeviceState(g_D3DD);

    m_DeviceState->StoreState();

    for (int i=0;i<m_SideCnt;++i)
    {
        if (m_Side[i].m_ColorTexture != NULL)
        {
            g_Cache->Destroy(m_Side[i].m_ColorTexture);
            m_Side[i].m_ColorTexture = NULL;
        }
    }
    
    if (m_NeutralSideColorTexture != NULL)
    {
        g_Cache->Destroy(m_NeutralSideColorTexture);
        m_NeutralSideColorTexture = NULL;
    }

    CMatrixEffect::ReleasePoolDefaultResources();

    WaterClear();
    if (IS_VB(m_ShadowVB)) DESTROY_VB(m_ShadowVB);

    m_Minimap.StoreTexture();

    // clear vbs and ibs
    CMatrixFlyer::MarkAllBuffersNoNeed();
    CMatrixMapGroup::MarkAllBuffersNoNeed();
    CVectorObject::MarkAllBuffersNoNeed();
    CTerSurface::MarkAllBuffersNoNeed();
    CVOShadowProj::MarkAllBuffersNoNeed();
    CVOShadowStencil::MarkAllBuffersNoNeed();
    CInstDraw::MarkAllBuffersNoNeed();
    SInshorewave::MarkAllBuffersNoNeed();
    m_DI.OnLostDevice();
    CBaseTexture::OnLostDevice();

    RESETFLAG(m_Flags, MMFLAG_VIDEO_RESOURCES_READY);
}

bool    CMatrixMap::CheckLostDevice(void)
{
    DTRACE();

    HRESULT hr = g_D3DD->TestCooperativeLevel();
    switch(hr)
    {
    case D3D_OK:
        if (!FLAG(m_Flags, MMFLAG_VIDEO_RESOURCES_READY))
        {
            CreatePoolDefaultResources(false);
        }
        break;
    case D3DERR_DEVICELOST:
        if (FLAG(m_Flags, MMFLAG_VIDEO_RESOURCES_READY))
        {
            ReleasePoolDefaultResources();
        }
        return true;

    case D3DERR_DEVICENOTRESET:
        if (FLAG(m_Flags, MMFLAG_VIDEO_RESOURCES_READY))
        {
            ReleasePoolDefaultResources();
        }

//        D3DResource::Dump(D3DRESTYPE_VB);
        hr = g_D3DD->Reset( &g_D3Dpp );
        if (hr != D3D_OK)
        {
            return true;
        } else
        {
            CreatePoolDefaultResources(false);
            break;
        }
        
    default:
        break;
    }

    return false;
}