#include "stdafx.h"
#include "yy.h"
#include "modelCreator.h"

#include "MainFrm.h"

#include <cmath>
//#include "yy_fs.h"
//#include "yy_window.h"
//#include "yy_model.h"
//
//enum class face_type {
//	p,
//	pu,
//	pun,
//	pn
//};
//struct simple_arr {
//	s32 data[0x100];
//	u32 sz = 0;
//	void push_back(s32 v) { data[sz++] = v; }
//	u32 size() { return sz; }
//	void reset() { sz = 0; }
//};
//struct face {
//	simple_arr p, u, n;
//	face_type ft = face_type::pun;
//	void reset() {
//		ft = face_type::pun;
//		p.reset();
//		u.reset();
//		n.reset();
//	}
//};
//struct VERTEX {
//	v4f position;
//	v4f texcoord;
//	v4f normal;
//};
//
//u8 * nextLine(u8 * ptr);
//u8 * skipSpaces(u8 * ptr);
//u8 * readVec2(u8 * ptr, v4f& vec2);
//u8 * readFloat(u8 * ptr, f32& value);
//u8 * readVec3(u8 * ptr, v4f& vec3);
//u8 * readFace(u8 * ptr, face& f, char * s);
//u8 * readWord(u8 * ptr, yyStringA& str);
//
//yyModel* CMainFrame::_importOBJ(const wchar_t* filePath, CString& outName)
//{
//	FILE* file = _wfopen(filePath, L"rb");
//	if (!file)
//	{
//		::MessageBoxW(0, L"Can't open file", filePath, 0);
//		return nullptr;
//	}
//
//	auto file_size = (size_t)yyFS::file_size(filePath);
//	if (!file_size)
//	{
//		::MessageBoxW(0,L"Bad file size %s\n", filePath,0);
//		return nullptr;
//	}
//
//	yyArray<u8> file_byte_array;
//	file_byte_array.reserve((u32)file_size + 2);
//	file_byte_array.setSize((u32)file_size + 2);
//
//	u8 * ptr = file_byte_array.data();
//	fread(ptr, 1, file_size, file);
//	ptr[(u32)file_size] = ' ';
//	ptr[(u32)file_size + 1] = 0;
//
//	bool groupBegin = false;
//	bool isModel = false;
//	bool grpFound = false;
//
//	v4f tcoords;
//	v4f pos;
//	v4f norm;
//
//	/*  obj ���� ����� ���� ����������������, ���������� v vn vt ����� �� ���������.
//		������� ����� ��� v vn vt � ��� �������
//	*/
//	yyArray<v4f> position;
//	yyArray<v4f> uv;
//	yyArray<v4f> normal;
//
//	position.reserve(0xffff);
//	uv.reserve(0xffff);
//	normal.reserve(0xffff);
//	
//	position.setAddMemoryValue(0xffff);
//	uv.setAddMemoryValue(0xffff);
//	normal.setAddMemoryValue(0xffff);
//
//	/*  ����� ����� ����� f ����� ����� ��������� ������������.
//		������� � ������� ����� ������ ����� � ���� ��������.	*/
//	yyArray<yyVertexModel> modelVerts;
//	modelVerts.reserve(0xffff);
//	modelVerts.setAddMemoryValue(0xffff);
//	yyArray<u32> modelInds;
//	modelInds.reserve(0xffff);
//	modelInds.setAddMemoryValue(0xffff);
//
//	yyStringA name_word;
//
//	face f;
//	char s[0xff];
//	
//	yyModel* newModel = yyCreate<yyModel>();
//
//	// ������� ���������� v vt vn ������� ��� ������
//	// ����� ��� ������ ���� ������� ������� �� ������ �����
//	v4i last_counter;
//	
//	while (*ptr)
//	{
//		switch (*ptr)
//		{
//		case '#':
//		case 's':
//		case 'l':
//		case 'u'://usemtl
//		case 'c'://curv
//		case 'm'://mtllib
//		case 'p'://parm
//		case 'd'://deg
//		case 'e'://end
//			ptr = nextLine(ptr);
//			break;
//		case 'v':
//		{
//			++ptr;
//			if (groupBegin)
//				groupBegin = false;
//			switch (*ptr)
//			{
//			case 't':
//				ptr = readVec2(++ptr, tcoords);
//				uv.push_back(tcoords);
//				++last_counter.y;
//				break;
//			case 'n':
//				ptr = readVec3(++ptr, norm);
//				normal.push_back(norm);
//				++last_counter.z;
//				break;
//			default:
//				ptr = readVec3(ptr, pos);
//				position.push_back(pos);
//				++last_counter.x;
//
//				newModel->m_aabb.add(pos);
//
//				break;
//			}
//		}break;
//		case 'f':
//		{
//			isModel = true;
//			f.reset();
//			ptr = readFace(++ptr, f, s);
//
//			// �������� ���������� ��� ������������
//			for (u32 sz2 = f.p.size() - 2, i2 = 0; i2 < sz2; ++i2)
//			{
//				auto index1 = 0;
//				auto index2 = 1 + i2;
//				auto index3 = 2 + i2;
//				auto pos_index1 = f.p.data[index1];
//				auto pos_index2 = f.p.data[index2];
//				auto pos_index3 = f.p.data[index3];
//
//					// ���� ������ ������������� �� �� ��������� �� ������� ������������ ���������� ��������
//					// -1 = ��������� �������
//				if (pos_index1 < 0) pos_index1 = last_counter.x + pos_index1 + 1;
//				if (pos_index2 < 0) pos_index2 = last_counter.x + pos_index2 + 1;
//				if (pos_index3 < 0) pos_index3 = last_counter.x + pos_index3 + 1;
//
//				yyVertexModel newVertex1;
//				yyVertexModel newVertex2;
//				yyVertexModel newVertex3;
//				newVertex1.Position = position[pos_index1];
//				newVertex2.Position = position[pos_index2];
//				newVertex3.Position = position[pos_index3];
//
//				if (f.ft == face_type::pu || f.ft == face_type::pun)
//				{
//					auto uv_index1 = f.u.data[index1];
//					auto uv_index2 = f.u.data[index2];
//					auto uv_index3 = f.u.data[index3];
//					if (uv_index1 < 0) uv_index1 = last_counter.y + uv_index1 + 1;
//					if (uv_index2 < 0) uv_index2 = last_counter.y + uv_index2 + 1;
//					if (uv_index3 < 0) uv_index3 = last_counter.y + uv_index3 + 1;
//
//					auto u1 = uv[uv_index1];
//					auto u2 = uv[uv_index2];
//					auto u3 = uv[uv_index3];
//					newVertex1.TCoords.set(u1.x, u1.y);
//					newVertex2.TCoords.set(u2.x, u2.y);
//					newVertex3.TCoords.set(u3.x, u3.y);
//				}
//
//				if (f.ft == face_type::pn || f.ft == face_type::pun)
//				{
//					auto nor_index1 = f.n.data[index1];
//					auto nor_index2 = f.n.data[index2];
//					auto nor_index3 = f.n.data[index3];
//					if (nor_index1 < 0) nor_index1 = last_counter.z + nor_index1 + 1;
//					if (nor_index2 < 0) nor_index2 = last_counter.z + nor_index2 + 1;
//					if (nor_index3 < 0) nor_index3 = last_counter.z + nor_index3 + 1;
//
//					newVertex1.Normal = normal[nor_index1];
//					newVertex2.Normal = normal[nor_index2];
//					newVertex3.Normal = normal[nor_index3];
//				}
//
//				/*	� ������ ����� ������� 3 ������� ������������. ���� ���������, �������� �� ����� ������� � ����-�� �� ����������.
//					���� ��, �� ���� ������ � ��� ��� � ��������� ������.
//					���� ���, �� ��� ������� � ������ � ��������� � ��������� ����� ������.								*/
//				u32 vIndex1 = 0xffffffff;
//				u32 vIndex2 = 0xffffffff;
//				u32 vIndex3 = 0xffffffff;
//				yyVertexModel* v1_ptr = &newVertex1;
//				yyVertexModel* v2_ptr = &newVertex2;
//				yyVertexModel* v3_ptr = &newVertex3;
//				for (u32 k = 0, ksz = modelVerts.size(); k < ksz; ++k)
//				{
//					auto _v = &modelVerts[k];
//					if (_v->Position == v1_ptr->Position
//						&& _v->Normal == v1_ptr->Normal
//						&& _v->TCoords == v1_ptr->TCoords) { v1_ptr = _v; vIndex1 = k; }
//					if (_v->Position == v2_ptr->Position
//						&& _v->Normal == v2_ptr->Normal
//						&& _v->TCoords == v2_ptr->TCoords) { v2_ptr = _v; vIndex2 = k; }
//					if (_v->Position == v3_ptr->Position
//						&& _v->Normal == v3_ptr->Normal
//						&& _v->TCoords == v3_ptr->TCoords) { v3_ptr = _v; vIndex3 = k; }
//				}
//
//				if (vIndex1 == 0xffffffff) { vIndex1 = modelVerts.size(); modelVerts.push_back(*v1_ptr); }
//				if (vIndex2 == 0xffffffff) { vIndex2 = modelVerts.size(); modelVerts.push_back(*v2_ptr); }
//				if (vIndex3 == 0xffffffff) { vIndex3 = modelVerts.size(); modelVerts.push_back(*v3_ptr); }
//
//				modelInds.push_back(vIndex1);
//				modelInds.push_back(vIndex2);
//				modelInds.push_back(vIndex3);
//			}
//
//		}break;
//		case 'o':
//		case 'g':
//		{
//			if (!groupBegin)
//				groupBegin = true;
//			else
//			{
//				ptr = nextLine(ptr);
//				break;
//			}
//
//			yyStringA tmp_word;
//			ptr = readWord(++ptr, tmp_word);
//			if (tmp_word.size())
//			{
//				if (!name_word.size())
//					name_word = tmp_word;
//			}
//
//			grpFound = true;
//		}break;
//		default:
//			++ptr;
//			break;
//		}
//	}
//
//	if (name_word.size())
//	{
//		outName.Empty();
//		outName = name_word.data();
//	}
//
//	// ������ ����� ������� ������
//	newModel->m_vertices = (u8*)yyMemAlloc(modelVerts.size() * sizeof(yyVertexModel));
//	memcpy(newModel->m_vertices, modelVerts.data(), modelVerts.size() * sizeof(yyVertexModel));
//
//	if (modelInds.size() / 3 > 21845)
//	{
//		newModel->m_indexType = yyMeshIndexType::u32;
//		newModel->m_indices = (u8*)yyMemAlloc(modelInds.size() * sizeof(u32));
//		memcpy(newModel->m_indices, modelInds.data(), modelInds.size() * sizeof(u32));
//	}
//	else
//	{
//		newModel->m_indexType = yyMeshIndexType::u16;
//		newModel->m_indices = (u8*)yyMemAlloc(modelInds.size() * sizeof(u16));
//		u16 * i_ptr = (u16*)newModel->m_indices;
//		for (u32 i = 0, sz = modelInds.size(); i < sz; ++i)
//		{
//			*i_ptr = (u16)modelInds[i];
//			++i_ptr;
//		}
//	}
//
//	newModel->m_name = name_word.data();
//	newModel->m_vCount = modelVerts.size();
//	newModel->m_iCount = modelInds.size();
//	newModel->m_stride = sizeof(yyVertexModel);
//	newModel->m_vertexType = yyVertexType::Model;
//
//	return newModel;
//}
//
//u8 * nextLine(u8 * ptr)
//{
//	while (*ptr)
//	{
//		if (*ptr == '\n')
//		{
//			ptr++;
//			return ptr;
//		}
//		ptr++;
//	}
//	return ptr;
//}
//
//u8 * readVec2(u8 * ptr, v4f& vec2)
//{
//	ptr = skipSpaces(ptr);
//	f32 x, y;
//	if (*ptr == '\n')
//	{
//		ptr++;
//	}
//	else
//	{
//		ptr = readFloat(ptr, x);
//		ptr = skipSpaces(ptr);
//		ptr = readFloat(ptr, y);
//		ptr = nextLine(ptr);
//		vec2.x = x;
//		vec2.y = y;
//	}
//	return ptr;
//}
//
//u8 * skipSpaces(u8 * ptr)
//{
//	while (*ptr)
//	{
//		if (*ptr != '\t' && *ptr != ' ')
//			break;
//		ptr++;
//	}
//	return ptr;
//}
//
//u8 * readFloat(u8 * ptr, f32& value)
//{
//	char str[32u];
//	memset(str, 0, 32);
//	char * p = &str[0u];
//	while (*ptr) {
//		if (!isdigit(*ptr) && (*ptr != '-') && (*ptr != '+')
//			&& (*ptr != 'e') && (*ptr != 'E') && (*ptr != '.')) break;
//		*p = *ptr;
//		++p;
//		++ptr;
//	}
//	value = (f32)atof(str);
//	return ptr;
//}
//
//u8 * readVec3(u8 * ptr, v4f& vec3) {
//	ptr = skipSpaces(ptr);
//	f32 x, y, z;
//	if (*ptr == '\n') {
//		ptr++;
//	}
//	else {
//		ptr = readFloat(ptr, x);
//		ptr = skipSpaces(ptr);
//		ptr = readFloat(ptr, y);
//		ptr = skipSpaces(ptr);
//		ptr = readFloat(ptr, z);
//		ptr = nextLine(ptr);
//		vec3.x = x;
//		vec3.y = y;
//		vec3.z = z;
//	}
//	return ptr;
//}
//
//u8 * skipSpace(u8 * ptr) {
//	while (*ptr) {
//		if (*ptr != ' ' && *ptr != '\t') break;
//		ptr++;
//	}
//	return ptr;
//}
//
//u8 * getInt(u8 * p, s32& i)
//{
//	char str[8u];
//	memset(str, 0, 8);
//	char * pi = &str[0u];
//
//	while (*p)
//	{
//		/*if( *p == '-' )
//		{
//		++p;
//		continue;
//		}*/
//
//		if (!isdigit(*p) && *p != '-') break;
//
//
//		*pi = *p;
//		++pi;
//		++p;
//	}
//	i = atoi(str);
//	return p;
//}
//
//u8 * readFace(u8 * ptr, face& f, char * s) {
//	ptr = skipSpaces(ptr);
//	if (*ptr == '\n')
//	{
//		ptr++;
//	}
//	else
//	{
//		while (true)
//		{
//			s32 p = 1;
//			s32 u = 1;
//			s32 n = 1;
//
//			ptr = getInt(ptr, p);
//
//			if (*ptr == '/')
//			{
//				ptr++;
//				if (*ptr == '/')
//				{
//					ptr++;
//					f.ft = face_type::pn;
//					ptr = getInt(ptr, n);
//				}
//				else
//				{
//					ptr = getInt(ptr, u);
//					if (*ptr == '/')
//					{
//						ptr++;
//						f.ft = face_type::pun;
//						ptr = getInt(ptr, n);
//					}
//					else
//					{
//						f.ft = face_type::pu;
//					}
//				}
//			}
//			else
//			{
//				f.ft = face_type::p;
//			}
//			f.n.push_back(n - 1);
//			f.u.push_back(u - 1);
//			f.p.push_back(p - 1);
//			ptr = skipSpace(ptr);
//
//			if (*ptr == '\r')
//				break;
//			else if (*ptr == '\n')
//				break;
//		}
//	}
//	return ptr;
//}
//
//u8 * readWord(u8 * ptr, yyStringA& str)
//{
//	ptr = skipSpaces(ptr);
//	str.clear();
//	while (*ptr)
//	{
//		if (isspace(*ptr))
//			break;
//		str += (char)*ptr;
//		ptr++;
//	}
//	return ptr;
//}