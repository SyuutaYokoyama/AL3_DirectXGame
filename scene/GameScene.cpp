﻿#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include<random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() { delete model_; }

void GameScene::Initialize() {
	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);
	worldTransform_->translation_ = {0.0f, 10.0f, 0.0f};
	viewProjection_.target = {10, 0, 0};
	viewProjection_.Initialize();
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	textureHandle_ = TextureManager::Load("mario.jpg");
	sprite_ = Sprite::Create(textureHandle_, {100, 100});
	textureHandle_ = TextureManager::Load("mario.jpg");
	model_ = Model::Create();
	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};
		worldTransform_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};
		worldTransform_[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};
		worldTransform_[i].Initialize();
	}
	
	soundDataHandle_ = audio_->LoadWave("se_sad03.wav");
	//audio_->PlayWave(soundDataHandle_);
	//voiceHandle_ = audio_->PlayWave(soundDataHandle_, true);
}

void GameScene::Update() {
	XMFLOAT2 position = sprite_->GetPosition();
	position.x += 2.0f;
	position.y += 1.0f;
	sprite_->SetPosition(position);
	if (input_->TriggerKey(DIK_SPACE)) {
		audio_->StopWave(voiceHandle_);
	}
	// std::string strDebug = std::string("transration:4.0") + std::to_string(value);
	//  debugText_->Printf("year:%d",2001);
	// std::string strDebug = std::string("transration:4.0") + std::to_string(translation_);
	// std::string strDebug2 = std::string("rotation:") + std::to_string(rotation_);
	// std::string strDebug3 = std::string("transration:") + std::to_string(scale_);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//debugText_->SetPos(50, 70);
	//debugText_->Printf(
	 // "translation:(%f,%f,%f)", worldTransform_.translation_.x, worldTransform_.translation_.y,
	 // worldTransform_.translation_.z);
	//debugText_->SetPos(50, 90);
	//debugText_->Printf(
	  //"rotation:(%f,%f,%f)", worldTransform_.rotation_.x, worldTransform_.rotation_.y,
	  //worldTransform_.rotation_.z);
	//debugText_->SetPos(50, 110);
	//debugText_->Printf(
	 //"scale:(%f,%f,%f)", worldTransform_.scale_.x, worldTransform_.scale_.y,
	  //worldTransform_.scale_.z);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// debugText_->Print(strDebug2, 50, 70, 1.0f);
	// debugText_->Print(strDebug3, 50, 90, 1.0f);
	XMFLOAT3 move = {0, 0, 0};
	const float kEyeSpeed = 0.2f;
	const float kTargetSpeed = 0.2f;
	if (input_->PushKey(DIK_W)) {
		move = {0, 0, kEyeSpeed};
	} else if (input_->PushKey(DIK_S)) {
		move = {0, 0, -kEyeSpeed};
	}
	if (input_->PushKey(DIK_LEFT)) {
		move = {-kTargetSpeed, 0, 0};
	} else if (input_->PushKey(DIK_RIGHT)) {
		move = {kTargetSpeed, 0, 0};
	}
	viewProjection_.eye.x += move.x;
	viewProjection_.eye.y += move.y;
    viewProjection_.eye.z += move.z;
	viewProjection_.target.x += move.x;
	viewProjection_.target.y += move.y;
	viewProjection_.target.z += move.z;

	viewProjection_.UpdateMatrix();

	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
	debugText_->SetPos(50, 70);
	debugText_->Printf(
	  "target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y, viewProjection_.target.z);
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	//sprite_->Draw();
	//   デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
