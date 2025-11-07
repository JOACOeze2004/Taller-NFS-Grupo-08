#include "text_renderer.h"

TextRenderer::TextRenderer() : font(nullptr) {}

TextRenderer::TextRenderer(const std::string& font_path, int ptsize) : font(nullptr) {
    load(font_path, ptsize);
}

TextRenderer::~TextRenderer() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}

TextRenderer::TextRenderer(TextRenderer&& other) noexcept : font(other.font) {
    other.font = nullptr;
}

TextRenderer& TextRenderer::operator=(TextRenderer&& other) noexcept {
    if (this != &other) {
        if (font) {
            TTF_CloseFont(font);
        }
        font = other.font;
        other.font = nullptr;
    }
    return *this;
}

bool TextRenderer::load(const std::string& font_path, int ptsize) {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    font = TTF_OpenFont(font_path.c_str(), ptsize);
    if (!font) {
        std::cerr << "[CLIENT] Error abriendo fuente '" << font_path << "': " << TTF_GetError() << std::endl;
        return false;
    }
    return true;
}

bool TextRenderer::ok() const { return font != nullptr; }

void TextRenderer::render(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color) {
    if (!font) return;
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "[CLIENT] Error creando surface de texto: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "[CLIENT] Error creando textura de texto: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    SDL_Rect dst{ x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}
