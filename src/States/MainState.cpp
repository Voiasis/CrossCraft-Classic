#include "MainState.h"
#include <Utilities/Input.h>
#include <GFX/RenderCore.h>

MainState::MainState() {

}

void MainState::init() {
	player = std::make_shared<Player>();
	textRenderer = std::make_unique<GFX::UI::TextRenderer>();
	textRenderer->init("./assets/font.pgf");
	textRenderer->setStyle({ 255, 255, 255, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_CENTER, 0.0f, 0 });

	world = std::make_unique<World>(player);
}

void MainState::cleanup() {
	textRenderer->cleanup();
}

void MainState::enter() {

}

void MainState::pause() {

}

void MainState::resume() {

}

void MainState::update(Core::GameStateManager*) {
	double dt = Utilities::g_AppTimer.deltaTime();
	player->update(dt);
}

void MainState::uiPass() {
	textRenderer->setStyle({ 255, 255, 255, 255, 0.5f, TEXT_RENDERER_CENTER, TEXT_RENDERER_LEFT, 0.0f, 0 });
	textRenderer->draw("X/Y/Z: ", { 0, 12 });

	textRenderer->draw(std::to_string(player->pos.x), { 0, 12 + 12 * 1 });
	textRenderer->draw(std::to_string(player->pos.y), { 0, 12 + 12 * 2 });
	textRenderer->draw(std::to_string(player->pos.z), { 0, 12 + 12 * 3 });

	textRenderer->draw("Pitch/Yaw: ", { 0, 12 + 12 * 5 });
	textRenderer->draw(std::to_string(player->rot.x), { 0, 12 + 12 * 6 });
	textRenderer->draw(std::to_string(player->rot.y), { 0, 12 + 12 * 7 });

	player->draw();
	world->update();
}

void MainState::mainPass(){
	world->draw();
}

void MainState::draw(Core::GameStateManager*) {

	//MAIN PASS
	GFX::g_RenderCore->bindCamera(player->cam);
	GFX::g_RenderCore->set3DMode();
	mainPass();


	//UI PASS
	GFX::g_RenderCore->setDefault2DMode();
	uiPass();
}
