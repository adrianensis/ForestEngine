#include "Batch.h"
#include "Material.h"
#include "Texture.h"
#include "List.h"
#include "Renderer.h"
#include "RenderEngine.h"
#include "Camera.h"
#include "Frustum.h"
#include "Mesh.h"
#include "Shader.h"
#include "RenderContext.h"
#include "List.h"
#include "Log.h"
#include "GameObject.h"
#include "Transform.h"
#include "Matrix4.h"
#include "MathUtils.h"
#include "Time.h"
#include "Chunk.h"

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
	mRenderersToRemove = nullptr;
	mMaterial = nullptr;
	mRenderEngine = nullptr;
	mTextureId = 0;
}

Batch::~Batch() {

	FOR_LIST(it, mRenderers){
    Memory::free<Renderer>(it.get());
  }

	Memory::free<List<Renderer*>>(mRenderers); // TODO : sorted by layers, or use a hashmap
	Memory::free<List<Renderer*>>(mRenderersToRemove);

	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBOPosition);
	glDeleteBuffers(1, &mEBO);
}

// ---------------------------------------------------------------------------

void Batch::init(Mesh* mesh, Material* material) {
	TRACE();

	mRenderEngine = RenderEngine::getInstance();

	mRenderers = Memory::allocate<List<Renderer*>>();
	mRenderers->init();

	mRenderersToRemove = Memory::allocate<List<Renderer*>>();
	mRenderersToRemove->init();

	mMesh = mesh;
	mMaterial = material;
}

// ---------------------------------------------------------------------------

void Batch::bind() {
	mVAO = RenderContext::createVAO();
	mVBOPosition = RenderContext::createVBO(mMesh->getVertices(), 3, 0);
	mVBOTexture = RenderContext::createVBO(mMesh->getTextureCoordinates(), 2, 1);
	//mVBOColor = RenderContext::createVBO(mMesh->getColors(), 4, 2);
//	mVBONormal = RenderContext::createVBO(mMesh->getNormals(), 3, 3);
	mEBO = RenderContext::createEBO(mMesh->getFaces());

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

bool Batch::checkInFrustum(Camera* cam, Renderer* renderer){
	Transform* t = renderer->getGameObject()->getTransform();

	Vector3 scale = t->getScale();
	f32 maxRadius = std::max(scale.x, scale.y); // TODO: if 3D, compare also with z

	Vector3 position(Vector3(t->getWorldPosition()).add(renderer->getPositionOffset()));

	return cam->getFrustum()->testSphere(position, maxRadius);
}

// ---------------------------------------------------------------------------

bool Batch::checkDistance(Camera* cam, Renderer* renderer){
	Transform* t = renderer->getGameObject()->getTransform();

	Vector3 camPosition(cam->getGameObject()->getTransform()->getLocalPosition());
	Vector3 rendererPosition(t->getLocalPosition());

	return rendererPosition.dst(camPosition) < renderer->getRenderDistance();
}

// ---------------------------------------------------------------------------

bool Batch::checkOutOfCamera(Camera* cam, Renderer* renderer){

	renderer->setOutOfCamera(!checkInFrustum(cam, renderer));

	return renderer->getOutOfCamera();
}

// ---------------------------------------------------------------------------

u32 Batch::render(u32 layer) {

	u32 drawCallCounter = 0;

	if(mRenderers->getLength() > 0 && mRenderers->get(0)->getChunk() && mRenderers->get(0)->getChunk()->isLoaded()){
		Shader* shader = mMaterial->getShader();

		shader->use();
		RenderContext::enableVAO(mVAO);
		glBindTexture(GL_TEXTURE_2D, mTextureId);

		Camera* camera = mRenderEngine->getCamera();

	  const Matrix4& projectionMatrix = camera->getProjectionMatrix();
	  const Matrix4& viewTranslationMatrix = camera->getViewTranslationMatrix();
	  const Matrix4& viewRotationMatrix = camera->getViewRotationMatrix();

	  shader->addMatrix(projectionMatrix, "projectionMatrix");
	  shader->addMatrix(viewTranslationMatrix, "viewTranslationMatrix");
	  shader->addMatrix(viewRotationMatrix, "viewRotationMatrix");

		// VAR(f32, mRenderers->getLength())

		FOR_LIST(it, mRenderers){

			Renderer* renderer = it.get();

			if(renderer->isActive()){
				
				Transform* t = renderer->getGameObject()->getTransform();

				bool chunkOk = (! renderer->getChunk()) || (renderer->getChunk() && renderer->getChunk()->isLoaded());

				if(renderer->getLayer() == layer && chunkOk && !checkOutOfCamera(camera,renderer)){

					const Matrix4& translationMatrix = t->getTranslationMatrix();
					const Matrix4& rotationMatrix = t->getRotationMatrix();
					const Matrix4& scaleMatrix = t->getScaleMatrix();

					shader->addMatrix(translationMatrix, "translationMatrix");
					shader->addMatrix(renderer->getPositionOffsetMatrix(), "positionOffsetMatrix");
					shader->addMatrix(rotationMatrix, "rotationMatrix");
					shader->addMatrix(scaleMatrix, "scaleMatrix");

					shader->addFloat(Time::getDeltaTimeSeconds(), "time");

					renderer->updateMaterial(mMaterial);

					bool lineMode = it.get()->isLineMode();

					glPolygonMode(GL_FRONT_AND_BACK, lineMode ? GL_LINE : GL_FILL);

					glDrawElements(GL_TRIANGLES, mMesh->getFaces()->getLength(), GL_UNSIGNED_INT, 0);

					drawCallCounter++;

				} else if(renderer->isPendingToBeDestroyed()) {
						// destroy renderer and remove from list
						internalRemoveRenderer(&it);
				}
			}
		}

		RenderContext::enableVAO(0);
	}

	return drawCallCounter;
}

// ---------------------------------------------------------------------------

void Batch::addRenderer(Renderer* renderer) {
		mRenderers->pushBack(renderer);
		renderer->setOutOfCamera(!checkDistance(mRenderEngine->getCamera(), renderer));

		// TODO insert sorted
}

// ---------------------------------------------------------------------------

void Batch::internalRemoveRenderer(const Iterator* it){
	auto castedIt = it->cast<Renderer*>();
	mRenderers->remove(*castedIt);

	Renderer* renderer = (*castedIt).get();
	renderer->setDestroyed();
	Memory::free<Renderer>(renderer);
}

// ---------------------------------------------------------------------------

} /* namespace DE */
