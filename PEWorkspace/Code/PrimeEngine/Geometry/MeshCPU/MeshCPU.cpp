// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Immediate header
#include "MeshCPU.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "../MaterialCPU/MaterialSetCPU.h"
#include "../TangentBufferCPU/TangentBufferCPU.h"
#include "../PositionBufferCPU/PositionBufferCPUManager.h"
#include "../NormalBufferCPU/NormalBufferCPUManager.h"
#include "../TexCoordBufferCPU/TexCoordBufferCPUManager.h"
// Sibling/Children includes

namespace PE {

// Reads the specified buffer from file
void MeshCPU::ReadMesh(const char *filename, const char *package, const char *tag)
{
    PEString::generatePathname(*m_pContext, filename, package, "Meshes", PEString::s_buf, PEString::BUF_SIZE);
	
	// Path is now a full path to the file with the filename itself
	FileReader f(PEString::s_buf);

	char line[256];
	f.nextNonEmptyLine(line, 255);
	// TODO : make sure it is "MESH"

	// Vertex buffer filename
	char vbfilename[256];
	f.nextNonEmptyLine(vbfilename, 255);
	StringOps::strcmp(vbfilename, "none");
	
	m_hPositionBufferCPU = PositionBufferCPUManager::Instance()->ReadVertexBuffer(vbfilename, package, tag);
	
	// Index buffer filename
	char ibfilename[256];
	f.nextNonEmptyLine(ibfilename, 255);
	assert(StringOps::strcmp(ibfilename, "none"));

	m_hIndexBufferCPU = PositionBufferCPUManager::Instance()->ReadIndexBuffer(ibfilename, package, tag);
	
	
	// TexCoord buffer filename
	char tcfilename[256];
	f.nextNonEmptyLine(tcfilename, 255);

	if (StringOps::strcmp(tcfilename, "none") != 0)
	{
		m_hTexCoordBufferCPU = PositionBufferCPUManager::Instance()->ReadTexCoordBuffer(tcfilename, package, tag);



		//TexCoordBufferCPU* p_hWindBufferCPU = m_hWindBufferCPU.getObject<TexCoordBufferCPU>();

		//TexCoordBufferCPU* obj_windBufferCPU = m_hWindBufferCPU.getObject<TexCoordBufferCPU>();

		//TexCoordBufferCPU obj_windBufferCPU = *(m_hTexCoordBufferCPU.getObject<TexCoordBufferCPU>());

		//m_hWindBufferCPU = 


		m_hWindBufferCPU = Handle("TEXCOORD_BUFFER_CPU", sizeof(TexCoordBufferCPU));
		TexCoordBufferCPU* obj_windBufferCPU = new(m_hWindBufferCPU) TexCoordBufferCPU(*m_pContext, m_arena);



		Array<PrimitiveTypes::Float32>* m_values = &(obj_windBufferCPU->m_values);

		TexCoordBufferCPU* ht = m_hTexCoordBufferCPU.getObject<TexCoordBufferCPU>();

		int tVals = ht->m_values.m_capacity;

		//Array<PrimitiveTypes::Float32>* t_values = 

		m_values->reset(tVals);
		//m_values->clear();

		//PrimitiveTypes::Float32* testVal =  m_values->m_dataHandle.getObject<PrimitiveTypes::Float32>();
		PrimitiveTypes::Float32* testVal = m_values->getFirstPtr();

		int i = 5;

		PositionBufferCPU* posBuf = m_hPositionBufferCPU.getObject<PositionBufferCPU>();


		

		
		Array<PrimitiveTypes::Float32>* pos_values = &(posBuf->m_values);

		//PrimitiveTypes::Float32* posTest = pos_values->m_dataHandle.getObject<PrimitiveTypes::Float32>();

		//m_values->clear();

		for (int i = 1, j = 0; i < pos_values->m_size; i += 3, j += 2) {
			m_values->add(pos_values->getByIndexUnchecked(i));
			//m_values->add(1);
			m_values->add(0);

			int x = 5;
		}

		//int y = 6;

		//int size = PositionBufferCPUManager::Instance()->getSize(tcfilename, package, tag);
		//Handle* heck = m_hWindBufferCPU.getObject();

		
	}
	else
	{
		// create mock
		m_hTexCoordBufferCPU = Handle("TEXCOORD_BUFFER_CPU", sizeof(TexCoordBufferCPU));
		TexCoordBufferCPU *ptcb = new(m_hTexCoordBufferCPU) TexCoordBufferCPU(*m_pContext, m_arena);
		ptcb->createMockCPUBuffer(m_hPositionBufferCPU.getObject<PositionBufferCPU>()->m_values.m_size / 3);
	}

	// Normal buffer filename
	char nbfilename[256];
	f.nextNonEmptyLine(nbfilename, 255);
	if(StringOps::strcmp(nbfilename, "none") != 0)
	{
		m_hNormalBufferCPU = NormalBufferCPUManager::Instance()->ReadNormalBuffer(nbfilename, package, tag);
	}

	// Tangent buffer filename
	char tbfilename[256];
	f.nextNonEmptyLine(tbfilename, 255);
	if(StringOps::strcmp(tbfilename, "none") != 0)
	{
		m_hTangentBufferCPU = PositionBufferCPUManager::Instance()->ReadTangentBuffer(tbfilename, package, tag);
	}
	/*
	this was enabled at some point. not sure why it was forcing having tangent buffer..
	else
	{
		m_hTangentBufferCPU = Handle(TANGENT_BUFFER_CPU, sizeof(TangentBufferCPU));
		TangentBufferCPU *ptb = new(m_hTangentBufferCPU) TangentBufferCPU();
		ptb->createMockCPUBuffer(m_hPositionBufferCPU.getObject<PositionBufferCPU>()->m_values.m_size / 3);
	}*/

	// MaterialSet filename
	char msfilename[256];
	f.nextNonEmptyLine(msfilename, 255);

	m_hMaterialSetCPU = PositionBufferCPUManager::Instance()->ReadMaterialSetCPU(msfilename, package);

	// Skin weights
	char swfilename[256];
	f.nextNonEmptyLine(swfilename, 255);
	if (StringOps::strcmp(swfilename, "none") != 0)
	{
		m_hSkinWeightsCPU = PositionBufferCPUManager::Instance()->ReadSkinWeights(swfilename, package, tag);
	}

	char optionalLine[256];
	while (f.nextNonEmptyLine(optionalLine, 255))
	{
		// have another line.. assume is additional meshes for blend shapes
		if (m_hAdditionalVertexBuffersCPU.m_capacity == 0)
		{
			m_hAdditionalVertexBuffersCPU.reset(16);
		}
		Handle additionalPositionBufferCPU = PositionBufferCPUManager::Instance()->ReadVertexBuffer(optionalLine, package, tag);
		m_hAdditionalVertexBuffersCPU.add(additionalPositionBufferCPU);
		
		f.nextNonEmptyLine(optionalLine, 255);
		if (m_hAdditionalTexCoordBuffersCPU.m_capacity == 0)
		{
			m_hAdditionalTexCoordBuffersCPU.reset(16);
		}
		
		Handle hAdditionalTexCoordBufferCPU = TexCoordBufferCPUManager::Instance()->ReadTexCoordBuffer(optionalLine, package, tag);
		m_hAdditionalTexCoordBuffersCPU.add(hAdditionalTexCoordBufferCPU);
	
		f.nextNonEmptyLine(optionalLine, 255);
		if (m_hAdditionalNormalBuffersCPU.m_capacity == 0)
		{
			m_hAdditionalNormalBuffersCPU.reset(16);
		}
		Handle hAdditionalNormalBufferCPU = NormalBufferCPUManager::Instance()->ReadNormalBuffer(optionalLine, package, tag);
		m_hAdditionalNormalBuffersCPU.add(hAdditionalNormalBufferCPU);
	}

	

}


void MeshCPU::createEmptyMesh()
{
	m_hPositionBufferCPU = Handle("VERTEX_BUFFER_CPU", sizeof(PositionBufferCPU));
	PositionBufferCPU *pvb = new(m_hPositionBufferCPU) PositionBufferCPU(*m_pContext, m_arena);
	pvb->createEmptyCPUBuffer();

	m_hIndexBufferCPU = Handle("INDEX_BUFFER_CPU", sizeof(IndexBufferCPU));
	IndexBufferCPU *pib = new(m_hIndexBufferCPU) IndexBufferCPU(*m_pContext, m_arena);
	pib->createEmptyCPUBuffer();

	m_hColorBufferCPU = Handle("COLOR_BUFFER_CPU", sizeof(ColorBufferCPU));
	ColorBufferCPU *pcb = new(m_hColorBufferCPU) ColorBufferCPU(*m_pContext, m_arena);
	pcb->createEmptyCPUBuffer();

	//m_hTexCoordBufferCPU = Handle(TEXCOORD_BUFFER_CPU, sizeof(TexCoordBufferCPU));
	//TexCoordBufferCPU *ptcb = new(m_hTexCoordBufferCPU) TexCoordBufferCPU();
	//ptcb->createBillboardCPUBuffer();

	//m_hNormalBufferCPU = Handle("NORMAL_BUFFER_CPU", sizeof(NormalBufferCPU));
	//NormalBufferCPU *pnb = new(m_hNormalBufferCPU) NormalBufferCPU();
	//pnb->createBillboardCPUBuffer();

	//m_hTangentBufferCPU

	m_hMaterialSetCPU = Handle("MATERIAL_SET_CPU", sizeof(MaterialSetCPU));
	MaterialSetCPU *pmscpu = new(m_hMaterialSetCPU) MaterialSetCPU(*m_pContext, m_arena);
	pmscpu->createSetWithOneDefaultMaterial();
}

void MeshCPU::createBillboardMesh()
{
	m_hPositionBufferCPU = Handle("VERTEX_BUFFER_CPU", sizeof(PositionBufferCPU));
	PositionBufferCPU *pvb = new(m_hPositionBufferCPU) PositionBufferCPU(*m_pContext, m_arena);
	pvb->createBillboardCPUBuffer(100.0f, 100.0f);
	
	m_hIndexBufferCPU = Handle("INDEX_BUFFER_CPU", sizeof(IndexBufferCPU));
	IndexBufferCPU *pib = new(m_hIndexBufferCPU) IndexBufferCPU(*m_pContext, m_arena);
	pib->createBillboardCPUBuffer();
	
	//m_hTexCoordBufferCPU = Handle(TEXCOORD_BUFFER_CPU, sizeof(TexCoordBufferCPU));
	//TexCoordBufferCPU *ptcb = new(m_hTexCoordBufferCPU) TexCoordBufferCPU();
	//ptcb->createBillboardCPUBuffer();

	m_hNormalBufferCPU = Handle("NORMAL_BUFFER_CPU", sizeof(NormalBufferCPU));
	NormalBufferCPU *pnb = new(m_hNormalBufferCPU) NormalBufferCPU(*m_pContext, m_arena);
	pnb->createBillboardCPUBuffer();
	
	//m_hTangentBufferCPU
	
	m_hMaterialSetCPU = Handle("MATERIAL_SET_CPU", sizeof(MaterialSetCPU));
	MaterialSetCPU *pmscpu = new(m_hMaterialSetCPU) MaterialSetCPU(*m_pContext, m_arena);
	pmscpu->createSetWithOneDefaultMaterial();
}

void MeshCPU::createBillboardMeshWithColorTexture(const char *textureFilename, const char *package, PrimitiveTypes::Float32 w, PrimitiveTypes::Float32 h, ESamplerState customSamplerState/* = SamplerState_Count*/)
{
	m_hPositionBufferCPU = Handle("VERTEX_BUFFER_CPU", sizeof(PositionBufferCPU));
	PositionBufferCPU *pvb = new(m_hPositionBufferCPU) PositionBufferCPU(*m_pContext, m_arena);
	pvb->createBillboardCPUBuffer(w, h);
	
	m_hIndexBufferCPU = Handle("INDEX_BUFFER_CPU", sizeof(IndexBufferCPU));
	IndexBufferCPU *pib = new(m_hIndexBufferCPU) IndexBufferCPU(*m_pContext, m_arena);
	pib->createBillboardCPUBuffer();
	
	m_hTexCoordBufferCPU = Handle("TEXCOORD_BUFFER_CPU", sizeof(TexCoordBufferCPU));
	TexCoordBufferCPU *ptcb = new(m_hTexCoordBufferCPU) TexCoordBufferCPU(*m_pContext, m_arena);
	ptcb->createBillboardCPUBuffer();

	m_hNormalBufferCPU = Handle("NORMAL_BUFFER_CPU", sizeof(NormalBufferCPU));
	NormalBufferCPU *pnb = new(m_hNormalBufferCPU) NormalBufferCPU(*m_pContext, m_arena);
	pnb->createBillboardCPUBuffer();
	
	//m_hTangentBufferCPU
	
	m_hMaterialSetCPU = Handle("MATERIAL_SET_CPU", sizeof(MaterialSetCPU));
	MaterialSetCPU *pmscpu = new(m_hMaterialSetCPU) MaterialSetCPU(*m_pContext, m_arena);
	pmscpu->createSetWithOneTexturedMaterial(textureFilename, package, customSamplerState);
}

void MeshCPU::createBillboardMeshWithColorGlowTextures(const char *colorTextureFilename, const char *glowTextureFilename, const char *package,
	PrimitiveTypes::Float32 w, PrimitiveTypes::Float32 h)
{
	m_hPositionBufferCPU = Handle("VERTEX_BUFFER_CPU", sizeof(PositionBufferCPU));
	PositionBufferCPU *pvb = new(m_hPositionBufferCPU) PositionBufferCPU(*m_pContext, m_arena);
	pvb->createBillboardCPUBuffer(w, h);
	
	m_hIndexBufferCPU = Handle("INDEX_BUFFER_CPU", sizeof(IndexBufferCPU));
	IndexBufferCPU *pib = new(m_hIndexBufferCPU) IndexBufferCPU(*m_pContext, m_arena);
	pib->createBillboardCPUBuffer();
	
	m_hTexCoordBufferCPU = Handle("TEXCOORD_BUFFER_CPU", sizeof(TexCoordBufferCPU));
	TexCoordBufferCPU *ptcb = new(m_hTexCoordBufferCPU) TexCoordBufferCPU(*m_pContext, m_arena);
	ptcb->createBillboardCPUBuffer();

	m_hNormalBufferCPU = Handle("NORMAL_BUFFER_CPU", sizeof(NormalBufferCPU));
	NormalBufferCPU *pnb = new(m_hNormalBufferCPU) NormalBufferCPU(*m_pContext, m_arena);
	pnb->createBillboardCPUBuffer();
	
	//m_hTangentBufferCPU
	
	m_hMaterialSetCPU = Handle("MATERIAL_SET_CPU", sizeof(MaterialSetCPU));
	MaterialSetCPU *pmscpu = new(m_hMaterialSetCPU) MaterialSetCPU(*m_pContext, m_arena);
	pmscpu->createSetWithOneTexturedMaterialWithGlow(colorTextureFilename, glowTextureFilename, package);
}
}; // namespace PE
