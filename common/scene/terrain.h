﻿#ifndef _YUYU_SCENE_TERRAIN_H_
#define _YUYU_SCENE_TERRAIN_H_

#include "containers/array.h"
#include "containers/list.h"
#include "math\aabb.h"
#include "math\triangle.h"

#include "yy_model.h"
#include "yy_fs.h"
#include "io\file_io.h"
#define GAME_TOOL
#include "io\xml_io.h"
#include "yy_material.h"
#include "scene\camera.h"

const s32 yyTerrainSectorSize = 40;
const f32 yyTerrainQuadSize = 5.f;

template<typename type>
class yyBinarySearchArray
{
	template<typename type>
	struct _node
	{
		_node()
			:
			m_left(nullptr),
			m_right(nullptr),
			m_value(0)//,
			//m_data(nullptr)
		{

		}
		_node* m_left;
		_node* m_right;
		u64 m_value;
		type m_data;
	};
	_node<type>* m_root;

	yyListFast<_node<type>*> m_nodes;
	u64 m_size;
public:
	yyBinarySearchArray()
		:
		m_root(0),
		m_size(0)
	{
	}
	~yyBinarySearchArray()
	{
		clear();
	}
	u64 size() { return m_size; }
	void clear()
	{
		m_size = 0;
		auto current = m_nodes.head();
		if (current)
		{
			auto _end = current->m_left;
			while (true)
			{
				auto next = current->m_right;
				yyDestroy(current->m_data);
				if (current == _end)
					break;
				current = next;
			}

			m_nodes.clear();
		}
	}
	type* get(u64 value)
	{
		if (m_root)
		{
			_node<type> * currentNode = m_root;
			while (true)
			{
				if (value == currentNode->m_value)
					return &currentNode->m_data;
				if (value < currentNode->m_value)
				{
					if (currentNode->m_left)
					{
						currentNode = currentNode->m_left;
						continue;
					}
					else return nullptr;
				}
				else 
				{
					if (currentNode->m_right)
					{
						currentNode = currentNode->m_right;
						continue;
					}
					else return nullptr;
				}
			}
		}
		return nullptr;
	}
	type* set(u64 value, const type& object)
	{
		if (m_root)
		{
			_node<type> * currentNode = m_root;
			while (true)
			{
				if (value == currentNode->m_value)
				{
					currentNode->m_data = object;
					return &currentNode->m_data;
				}

				if (value < currentNode->m_value)
				{
					if (currentNode->m_left)
					{
						currentNode = currentNode->m_left;
						continue;
					}
					else
					{
						auto newBranch = yyCreate<_node<type>>();
						newBranch->m_data = object;
						newBranch->m_value = value;
						currentNode->m_left = newBranch;
						m_nodes.push_back(newBranch);
						++m_size;
						return &newBranch->m_data;
					}
				}
				else
				{
					if (currentNode->m_right)
					{
						currentNode = currentNode->m_right;
						continue;
					}
					else
					{
						auto newBranch = yyCreate<_node<type>>();
						newBranch->m_data = object;
						newBranch->m_value = value;
						currentNode->m_right = newBranch;
						m_nodes.push_back(newBranch);
						++m_size;
						return &newBranch->m_data;
					}
				}
			}

		}
		else
		{
			m_root = yyCreate<_node<type>>();
			m_root->m_data = object;
			m_root->m_value = value;
			m_nodes.push_back(m_root);
			++m_size;
			return &m_root->m_data;
		}
		return nullptr;
	}
};

struct yyTerrainVertex
{
	yyTerrainVertex()
		//:
		//m_index(0)
	{

	}
	v3f m_position;
	v2f m_tcoords;

	//u32 m_index;
};
struct yyTerrainSector
{
	yyTerrainSector()
	{
		m_model = nullptr;
		m_modelGPU = nullptr;
		m_visible = false;
		m_currentIndex = 0;

		m_model = yyCreate<yyModel>();
		m_meshBuffer = yyCreate<yyMeshBuffer>();
		m_meshBuffer->m_indexType = yyMeshIndexType::u16;
		m_meshBuffer->m_iCount = ((yyTerrainSectorSize * yyTerrainSectorSize * 2) * 3);
		
		m_indsSizeInBytes = m_meshBuffer->m_iCount * sizeof(u16);
		m_meshBuffer->m_indices = (u8*)yyMemAlloc(m_indsSizeInBytes);

		m_meshBuffer->m_stride = sizeof(yyVertexModel);
		m_meshBuffer->m_vertexType = yyVertexType::Model;
		m_meshBuffer->m_vCount = m_meshBuffer->m_iCount;

		m_vertsSizeInBytes = m_meshBuffer->m_vCount * m_meshBuffer->m_stride;
		m_meshBuffer->m_vertices = (u8*)yyMemAlloc(m_vertsSizeInBytes);

		m_model->m_meshBuffers.push_back(m_meshBuffer);

		m_sectorX = 0;
		m_sectorY = 0;

		m_physicsData = nullptr;
	}

	~yyTerrainSector()
	{
		if (m_physicsData)
		{
			yyDestroy(m_physicsData);
		}
		if (m_model)
		{
			yyDestroy(m_model);
		}
	}

	u32 m_currentIndex;

	void AddPiece(
		const yyTerrainVertex& _v1,
		const yyTerrainVertex& _v2,
		const yyTerrainVertex& _v3,
		const yyTerrainVertex& _v4,
		u8 type // type==0 ? v1,v2,v3\v1,v3,v4 : v2,v3,v4\v2,v4,v1
	)
	{
		auto verts = (yyVertexModel*)m_meshBuffer->m_vertices;
		auto inds = (u16*)m_meshBuffer->m_indices;

		u32 vertexIndex = m_currentIndex;

		inds[m_currentIndex] = m_currentIndex; ++m_currentIndex;
		inds[m_currentIndex] = m_currentIndex; ++m_currentIndex;
		inds[m_currentIndex] = m_currentIndex; ++m_currentIndex;
		inds[m_currentIndex] = m_currentIndex; ++m_currentIndex;
		inds[m_currentIndex] = m_currentIndex; ++m_currentIndex;
		inds[m_currentIndex] = m_currentIndex; ++m_currentIndex;

		if (type == 0)
		{
			verts[vertexIndex].Normal.set(0.f, 1.f, 0.f);
			verts[vertexIndex].TCoords = _v1.m_tcoords;
			verts[vertexIndex].Position = _v1.m_position; ++vertexIndex;
			verts[vertexIndex].Normal.set(0.f, 1.f, 0.f);
			verts[vertexIndex].TCoords = _v3.m_tcoords;
			verts[vertexIndex].Position = _v3.m_position; ++vertexIndex;
			verts[vertexIndex].Normal.set(0.f, 1.f, 0.f);
			verts[vertexIndex].TCoords = _v2.m_tcoords;
			verts[vertexIndex].Position = _v2.m_position; ++vertexIndex;

			verts[vertexIndex].Normal.set(0.f, 1.f, 0.f);
			verts[vertexIndex].TCoords = _v1.m_tcoords;
			verts[vertexIndex].Position = _v1.m_position; ++vertexIndex;
			verts[vertexIndex].Normal.set(0.f, 1.f, 0.f);
			verts[vertexIndex].TCoords = _v4.m_tcoords;
			verts[vertexIndex].Position = _v4.m_position; ++vertexIndex;
			verts[vertexIndex].Normal.set(0.f, 1.f, 0.f);
			verts[vertexIndex].TCoords = _v3.m_tcoords;
			verts[vertexIndex].Position = _v3.m_position; ++vertexIndex;
		}
		else
		{
			verts[vertexIndex].Normal.set(0.f, 1.f, 0.f);
			verts[vertexIndex].TCoords = _v2.m_tcoords;
			verts[vertexIndex].Position = _v2.m_position; ++vertexIndex;
			verts[vertexIndex].Normal.set(0.f, 1.f, 0.f);
			verts[vertexIndex].TCoords = _v1.m_tcoords;
			verts[vertexIndex].Position = _v1.m_position; ++vertexIndex;
			verts[vertexIndex].Normal.set(0.f, 1.f, 0.f);
			verts[vertexIndex].TCoords = _v4.m_tcoords;
			verts[vertexIndex].Position = _v4.m_position; ++vertexIndex;

			verts[vertexIndex].Normal.set(0.f, 1.f, 0.f);
			verts[vertexIndex].TCoords = _v2.m_tcoords;
			verts[vertexIndex].Position = _v2.m_position; ++vertexIndex;
			verts[vertexIndex].Normal.set(0.f, 1.f, 0.f);
			verts[vertexIndex].TCoords = _v4.m_tcoords;
			verts[vertexIndex].Position = _v4.m_position; ++vertexIndex;
			verts[vertexIndex].Normal.set(0.f, 1.f, 0.f);
			verts[vertexIndex].TCoords = _v3.m_tcoords;
			verts[vertexIndex].Position = _v3.m_position; ++vertexIndex;
		}
	}

	void CreateGPUResource()
	{
		if (m_modelGPU)
		{
			yyGetVideoDriverAPI()->UnloadModel(m_modelGPU);
			m_modelGPU = nullptr;
		} 
		m_modelGPU = yyGetVideoDriverAPI()->CreateModel(m_model);
		yyGetVideoDriverAPI()->UnloadModel(m_modelGPU);
		m_modelGPU_forRender = m_modelGPU;
	}
	yyModel* m_model;
	yyMeshBuffer* m_meshBuffer;
	yyResource* m_modelGPU;
	yyResource* m_modelGPU_forRender;
	yyMaterial m_material;

	
	u32 m_vertsSizeInBytes;
	u32 m_indsSizeInBytes;

	bool m_visible;
	Aabb m_aabb;
	v4f m_localPosition;

	s32 m_sectorX;
	s32 m_sectorY;

	struct meshbuffer_info
	{
		u32 m_vertsCount;
		u32 m_vertsStride;
		u32 m_indsCount;

		u32 m_compressedBufferSize;
	};

	void SaveToFile()
	{
		yyStringA fn;
		fn = "../res/world/";
		fn += "sector";
		fn += m_sectorX;
		fn += "_";
		fn += m_sectorY;
		fn += ".yytr";

		yyFileIO f;
		f.open(fn.c_str(), "wb");

		u32 buffer_size = m_vertsSizeInBytes + m_indsSizeInBytes;
		u8 * buffer = (u8*)yyMemAlloc(buffer_size);
		memcpy(&buffer[0], &m_meshBuffer->m_vertices[0], m_vertsSizeInBytes);
		memcpy(&buffer[m_vertsSizeInBytes], &m_meshBuffer->m_indices[0], m_indsSizeInBytes);

		u32 compressedBufferSize = 0;
		auto compressedBuffer = yyCompressData(buffer, buffer_size, compressedBufferSize, yyCompressType::ZStd);
		
		meshbuffer_info bufInfo;
		bufInfo.m_indsCount = m_meshBuffer->m_iCount;
		bufInfo.m_vertsCount = m_meshBuffer->m_vCount;
		bufInfo.m_vertsStride = m_meshBuffer->m_stride;
		bufInfo.m_compressedBufferSize = compressedBufferSize;

		f.writeBytes(&bufInfo, sizeof(meshbuffer_info));
		f.writeBytes(compressedBuffer, compressedBufferSize);

		yyMemFree(compressedBuffer);
		yyMemFree(buffer);

		//yyDecompressData()
	}
	void Unload()
	{
		if (m_meshBuffer->m_indices)
		{
			yyMemFree(m_meshBuffer->m_indices);
			m_meshBuffer->m_indices = nullptr;
		}
		if (m_meshBuffer->m_vertices)
		{
			yyMemFree(m_meshBuffer->m_vertices);
			m_meshBuffer->m_vertices = nullptr;
		}
	}
	void RecalculateAabb()
	{
		m_aabb.reset();

		yyVertexModel* verts = (yyVertexModel*)m_meshBuffer->m_vertices;
		for (u32 i = 0; i < m_meshBuffer->m_vCount; ++i)
		{
			m_aabb.add(verts[i].Position);
		}
		m_aabb.center(m_localPosition);
	}
	void Load()
	{
		yyStringA fn;
		fn = "../res/world/";
		fn += "sector";
		fn += m_sectorX;
		fn += "_";
		fn += m_sectorY;
		fn += ".yytr";

		yyFileIO f;
		f.open(fn.c_str(), "rb");

		meshbuffer_info bufInfo;
		f.readBytes(&bufInfo, sizeof(meshbuffer_info));

		if (!m_meshBuffer->m_indices)
		{
			m_meshBuffer->m_indices = (u8*)yyMemAlloc(m_indsSizeInBytes);
		}
		if (!m_meshBuffer->m_vertices)
		{
			m_meshBuffer->m_vertices = (u8*)yyMemAlloc(m_vertsSizeInBytes);
		}

		u8 * compressedBuffer = (u8*)yyMemAlloc(bufInfo.m_compressedBufferSize);
		f.readBytes(compressedBuffer, bufInfo.m_compressedBufferSize);
		u32 outSize = 0;
		auto decompressedBuffer = yyDecompressData(compressedBuffer, bufInfo.m_compressedBufferSize, outSize, yyCompressType::ZStd);
		
		yyMemFree(compressedBuffer);

		memcpy(m_meshBuffer->m_vertices, &decompressedBuffer[0], m_vertsSizeInBytes);
		memcpy(m_meshBuffer->m_indices, &decompressedBuffer[m_vertsSizeInBytes], m_indsSizeInBytes);
	
		yyMemFree(decompressedBuffer);
	}

	bool isRayIntersect(const yyRay& ray, v4f& ip)
	{
		yyTriangle triangle;
		yyVertexModel* verts = (yyVertexModel*)m_meshBuffer->m_vertices;
		u16* inds = (u16*)m_meshBuffer->m_indices;
		for (u32 i = 0; i < m_meshBuffer->m_iCount; i += 3)
		{
			triangle.v1 = verts[inds[i]].Position;
			triangle.v2 = verts[inds[i+1]].Position;
			triangle.v3 = verts[inds[i+2]].Position;
			triangle.update();

			f32 len = 0.f;
			f32 U, V, W = 0.f;
			if (triangle.rayTest_MT(ray, false, len, U,V,W))
			{
				ip = ray.m_origin + (ray.m_direction * len);
				return true;
			}
		}
		return false;
	}

	/*
		your struct with collision shape, rigid body and other physics engine information
		use m_physicsData = yyCreate<yourtype>()
	*/
	void* m_physicsData;
};

struct yyTerrain
{

	yyTerrain()
	{
	}

	~yyTerrain()
	{
		for (u16 i = 0, sz = m_sectors.size(); i < sz; ++i)
		{
			yyDestroy(m_sectors[i]);
		}
	}

	struct editableVerts
	{
		yyVertexModel* m_vertex;
		f32 m_weight;
	};
	yyArray<editableVerts> m_editableVerts;
	yyArray<yyTerrainSector*> m_updateSectors;
	void FindEditableVerts(const v4f& intersectionPoint, f32 sphere_radius)
	{
		f32 coef = 1.f / sphere_radius;

		m_updateSectors.clear();
		m_editableVerts.clear();
		m_editableVerts.setAddMemoryValue(0xffff);
		for (u16 i = 0, sz = m_sectors.size(); i < sz; ++i)
		{
			bool needUpdate = false;
			auto sector = m_sectors[i];
			if (sector->m_visible)
			{
				yyVertexModel* verts = (yyVertexModel*)sector->m_meshBuffer->m_vertices;
				for (u32 k = 0; k < sector->m_meshBuffer->m_vCount; ++k)
				{
					auto v = &verts[k];

					auto d = intersectionPoint.distance( 
						v4f(v->Position.x, v->Position.y, v->Position.z, 0.f)
					// если в будущем будет нужно переместить террейн, то тут нужно будет 
						// добавить ещё + позицию террейна
					);
					if (d <= sphere_radius)
					{
						editableVerts ev;
						ev.m_vertex = v;
						ev.m_weight = 1.f - (d * coef);
						m_editableVerts.push_back(ev);
						needUpdate = true;
					}
				}
			}
			if(needUpdate)
				m_updateSectors.push_back(sector);
		}
		printf("EDIT %u verts\n", m_editableVerts.size());
	}
	void EditMove(f32 mouseDelta_y, f32 deltaTime)
	{
		f32 speed = mouseDelta_y * deltaTime;
		if (m_editableVerts.size())
		{
			for (u32 i = 0, sz = m_editableVerts.size(); i < sz; ++i)
			{
				auto & ev = m_editableVerts[i];
				ev.m_vertex->Position.y -= speed * ev.m_weight;
			}
			for (u32 i = 0, sz = m_updateSectors.size(); i < sz; ++i)
			{
				auto sector = m_updateSectors[i];
				u8* vptr = 0;
				yyGetVideoDriverAPI()->MapModelForWriteVerts(sector->m_modelGPU, 0, &vptr);
				memcpy(vptr, sector->m_meshBuffer->m_vertices, sector->m_vertsSizeInBytes);
				yyGetVideoDriverAPI()->UnmapModelForWriteVerts(sector->m_modelGPU);
			}
		}
	}
	void Load(const char* xmlFile)
	{
		yyPtr<kkXMLDocument> xml = yyCreate<kkXMLDocument>();
		xml.m_data->Read(yyString(xmlFile));
		auto TERRAIN_node = xml.m_data->GetRootNode();
		auto x_sectors_attr = TERRAIN_node->getAttribute("x_sectors");
		auto y_sectors_attr = TERRAIN_node->getAttribute("y_sectors");
		s32 sectorsX = util::to_int(x_sectors_attr->value.c_str(), x_sectors_attr->value.size());
		s32 sectorsY = util::to_int(y_sectors_attr->value.c_str(), y_sectors_attr->value.size());

		for (s32 Y = 0; Y < sectorsY; ++Y)
		{
			for (s32 X = 0; X < sectorsX; ++X)
			{
				yyTerrainSector* newSector = yyCreate<yyTerrainSector>();
				newSector->m_sectorX = X;
				newSector->m_sectorY = Y;
				m_sectors.push_back(newSector);
				newSector->Load();
				newSector->RecalculateAabb();
				newSector->CreateGPUResource();
				newSector->Unload();
			}
		}
	}
	void SaveAfterEdit()
	{
		for (u32 i = 0, sz = m_updateSectors.size(); i < sz; ++i)
		{
			auto sector = m_updateSectors[i];
			sector->SaveToFile();
		}
		m_updateSectors.clear();
	}

	void Generate(s32 sectorsX, s32 sectorsY)
	{
		assert(sectorsX>0);
		assert(sectorsY>0);

		yyFS::create_directory(L"../res/");
		yyFS::create_directory(L"../res/world/");
		yyPtr<kkXMLDocument> xml = yyCreate<kkXMLDocument>();
		auto xmlRoot = xml.m_data->GetRootNode();
		xmlRoot->name = "TERRAIN";
		
		kkXMLAttribute* xmlattr_sectorsX = yyCreate<kkXMLAttribute>();
		xmlattr_sectorsX->name = "x_sectors";
		xmlattr_sectorsX->value += sectorsX;
		xmlRoot->addAttribute(xmlattr_sectorsX);

		kkXMLAttribute* xmlattr_sectorsY = yyCreate<kkXMLAttribute>();
		xmlattr_sectorsY->name = "y_sectors";
		xmlattr_sectorsY->value += sectorsY;
		xmlRoot->addAttribute(xmlattr_sectorsY);

		xml.m_data->Write((const char16_t*)L"../res/world/terrain.xml",true);

		f32 full_len_x = (f32)sectorsX * (f32)yyTerrainSectorSize * yyTerrainQuadSize;
		f32 full_len_y = (f32)sectorsY * (f32)yyTerrainSectorSize * yyTerrainQuadSize;

		f32 half_len_x = full_len_x * 0.5f;
		f32 half_len_y = full_len_y * 0.5f;

		v3f point(-half_len_x, 0.f, -half_len_y);

		


		for (s32 Y = 0; Y < sectorsY; ++Y)
		{
			// AABB должны помочь в установке позиции point 
			Aabb sectorRow_aabb;
			for (s32 X = 0; X < sectorsX; ++X)
			{
				yyTerrainSector* newSector = yyCreate<yyTerrainSector>();
				newSector->m_sectorX = X;
				newSector->m_sectorY = Y;
				m_sectors.push_back(newSector);

				u8 t = 0; // чтобы чередовать каким способом делать треугольники / или \

				float uv_segment_size_h = 1.f / (float)yyTerrainSectorSize;
				float uv_segment_size_w = 1.f / (float)yyTerrainSectorSize;
				float uv_begin_x = 0.f;
				float uv_begin_y = 1.f;

				for (s32 H = 0; H < yyTerrainSectorSize; ++H)
				{
					Aabb row_aabb;
					row_aabb.add(point);

					f32 x_begin = point.x;

					for (s32 W = 0; W < yyTerrainSectorSize; ++W)
					{
						// в сектор надо передать 4 точки для текущего кусочка террейна
						yyTerrainVertex v1, v2, v3, v4;
						v1.m_position.set(point.x, point.y, point.z);
						v2.m_position.set(point.x, point.y, point.z + yyTerrainQuadSize);
						v3.m_position.set(point.x + yyTerrainQuadSize, point.y, point.z + yyTerrainQuadSize);
						v4.m_position.set(point.x + yyTerrainQuadSize, point.y, point.z);

						v1.m_tcoords.set(uv_begin_x, uv_begin_y);
						v2.m_tcoords.set(uv_begin_x, uv_begin_y - uv_segment_size_h);
						v3.m_tcoords.set(uv_begin_x + uv_segment_size_w, uv_begin_y - uv_segment_size_h);
						v4.m_tcoords.set(uv_begin_x + uv_segment_size_w, uv_begin_y);

						uv_begin_x += uv_segment_size_w;

						newSector->AddPiece(v1, v2, v3, v4, t);
						point.x += yyTerrainQuadSize;
						row_aabb.add(point);

						t = (t == 0) ? 1 : 0;
					}
					t = (t == 0) ? 1 : 0;

					uv_begin_x = 0;
					uv_begin_y -= uv_segment_size_h;

					point.x = x_begin;
					point.z += yyTerrainQuadSize;

					newSector->m_aabb.add(row_aabb);

				}

				newSector->m_aabb.center(newSector->m_localPosition);
				newSector->SaveToFile();
				newSector->CreateGPUResource();
				newSector->Unload();


				sectorRow_aabb.add(newSector->m_aabb);
				point.x = sectorRow_aabb.m_max.x;
				point.z = sectorRow_aabb.m_min.z;
			}
			point.x = sectorRow_aabb.m_min.x;
			point.z = sectorRow_aabb.m_max.z + yyTerrainQuadSize;
		}
	}

	void OptimizeView(yyCamera* camera)
	{
		for (u16 i = 0, sz = m_sectors.size(); i < sz; ++i)
		{
			auto sector = m_sectors[i];
			if (camera->m_frustum.sphereInFrustum(sector->m_aabb.m_min.distance(sector->m_aabb.m_max) * 0.5f, 
				sector->m_localPosition
			))
			{
				sector->m_material.m_wireframe = false;
				
				const f32 dist_1 = 200.f;
				const f32 dist_2 = 1500.f;

				f32 distToCam = sector->m_localPosition.distance(camera->m_objectBase.m_globalPosition);
				if (distToCam < dist_1)
				{
					if (!sector->m_visible)
					{
						sector->Load();
						yyGetVideoDriverAPI()->LoadModel(sector->m_modelGPU);
					}
					sector->m_visible = true;
					sector->m_modelGPU_forRender = sector->m_modelGPU;
				}
				else if (distToCam < dist_2)
				{
					if (!sector->m_visible)
					{
						sector->Load();
						yyGetVideoDriverAPI()->LoadModel(sector->m_modelGPU);
					}
					sector->m_visible = true;
					sector->m_modelGPU_forRender = sector->m_modelGPU;
				}
				else
				{
					if (sector->m_visible)
					{
						yyGetVideoDriverAPI()->UnloadModel(sector->m_modelGPU);
						sector->Unload();
					}
					sector->m_visible = false;
					sector->m_modelGPU_forRender = sector->m_modelGPU;
				}

				if (   camera->m_objectBase.m_globalPosition.x + yyTerrainQuadSize >= sector->m_aabb.m_min.x
					&& camera->m_objectBase.m_globalPosition.z + yyTerrainQuadSize >= sector->m_aabb.m_min.z
					&& camera->m_objectBase.m_globalPosition.x <= sector->m_aabb.m_max.x + yyTerrainQuadSize
					&& camera->m_objectBase.m_globalPosition.z <= sector->m_aabb.m_max.z + yyTerrainQuadSize)
				{
					sector->m_material.m_wireframe = true;
				}

			}
			else
			{
				sector->m_visible = false;
			}

		}
	}

	yyArraySmall<yyTerrainSector*> m_sectors;
};

#endif