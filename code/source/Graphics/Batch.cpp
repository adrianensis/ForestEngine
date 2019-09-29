#include "Batch.h"
#include "Material.h"
#include "Texture.h"
#include "List.h"
#include "Renderer.h"
#include "RenderEngine.h"
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "RenderContext.h"
#include "List.h"
#include "Debug.h"
#include "GameObject.h"
#include "Transform.h"
#include "Matrix4.h"

namespace DE {

// ---------------------------------------------------------------------------

Batch::Batch() : DE_Class() {
	mVBOPosition = 0;
	mEBO = 0;
	mVBOTexture = 0;
	//mVBOColor = 0;
	mVBONormal = 0;
	mVAO = 0;
	mMesh = nullptr;
	mRenderers = nullptr;
	mMaterial = nullptr;
}

Batch::~Batch() {
	Memory::free<List<Renderer*>>(mRenderers); // TODO : sorted by layers, or use a hashmap

	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBOPosition);
	glDeleteBuffers(1, &mEBO);
}

// ---------------------------------------------------------------------------

void Batch::init(RenderEngine* renderEngine, Mesh* mesh, Material* material) {
	mRenderEngine = renderEngine;

	mRenderers = Memory::allocate<List<Renderer*>>();
	mRenderers->init();

	mMesh = mesh;
	mMaterial = material;
}

// ---------------------------------------------------------------------------

void Batch::bind() {
	mVAO = RenderContext::createVAO();
	mVBOPosition = RenderContext::createVBO(mMesh->getVerticesData(), 3, 0);
	mVBOTexture = RenderContext::createVBO(mMesh->getTexureCoordinatesData(), 2, 1);
	//mVBOColor = RenderContext::createVBO(mMesh->getColorsData(), 4, 2);
//	mVBONormal = RenderContext::createVBO(mMesh->getNormalsData(), 3, 3);
	mEBO = RenderContext::createEBO(mMesh->getFacesData());

	glGenTextures(1, &mTextureId);

	glBindTexture(GL_TEXTURE_2D, mTextureId);

	Texture* texture = mMaterial->getTexture();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->getWidth(), texture->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->getData());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// Prevents s-coordinate wrapping (repeating).
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// Prevents t-coordinate wrapping (repeating).
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	RenderContext::enableVAO(0);
}

// ---------------------------------------------------------------------------

void Batch::update() {

}

// ---------------------------------------------------------------------------

void Batch::render() {

	Shader* shader = mMaterial->getShader();

	shader->use();
	RenderContext::enableVAO(mVAO);
	glBindTexture(GL_TEXTURE_2D, mTextureId);

	const Matrix4& projectionMatrix = mRenderEngine->getCamera()->getProjectionMatrix();
	const Matrix4& viewTranslationMatrix = mRenderEngine->getCamera()->getViewTranslationMatrix();
	const Matrix4& viewRotationMatrix = mRenderEngine->getCamera()->getViewRotationMatrix();

	u32 i=0;
	for (auto it = mRenderers->getIterator(); !it.isNull(); it.next()){
		//ECHO("RENDERER NUM")
		//VAL(u32,i+1)

		const Matrix4& translationMatrix = it.get()->getGameObject()->getTransform()->getTranslationMatrix();
		const Matrix4& rotationMatrix = it.get()->getGameObject()->getTransform()->getRotationMatrix();
		const Matrix4& scaleMatrix = it.get()->getGameObject()->getTransform()->getScaleMatrix();

		shader->addMatrix(projectionMatrix, "projectionMatrix");
		shader->addMatrix(viewTranslationMatrix, "viewTranslationMatrix");
		shader->addMatrix(viewRotationMatrix, "viewRotationMatrix");

		shader->addMatrix(translationMatrix, "translationMatrix");
		shader->addMatrix(rotationMatrix, "rotationMatrix");
		shader->addMatrix(scaleMatrix, "scaleMatrix");

		it.get()->updateMaterial(mMaterial);

		glDrawElements(GL_TRIANGLES, mMesh->getFacesData()->getLength(), GL_UNSIGNED_INT, 0);
	}

	RenderContext::enableVAO(0);

}

// ---------------------------------------------------------------------------

void Batch::setMesh(Mesh* mesh){ mMesh = mesh; }
void Batch::setMaterial(Material* material){ mMaterial = material; }

// ---------------------------------------------------------------------------

void Batch::addRenderer(Renderer* renderer) {
	// u32 layerIndex=0;
	// for (auto it = mRenderers->getIterator(); !it.isNull(); it.next()){
	// 	u32 layer = it.get()->getGameObject()->getLayer();
	// 	if(layer == layerIndex){
	// 		// INSERT
	// 	}
	//
	// 	layerIndex++;
	// }

	mRenderers->pushBack(renderer);
}

// ---------------------------------------------------------------------------

} /* namespace DE */
