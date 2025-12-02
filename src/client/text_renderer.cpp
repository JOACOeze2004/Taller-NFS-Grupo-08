#include "text_renderer.h"

TextRenderer::TextRenderer() : font(nullptr), current_texture(nullptr) {}

TextRenderer::TextRenderer(const std::string& font_path, int ptsize) 
    : font(nullptr), current_texture(nullptr) {
    load(font_path, ptsize);
}

TextRenderer::~TextRenderer() {
    clearTexture();
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}

TextRenderer::TextRenderer(TextRenderer&& other) noexcept 
    : font(other.font), current_texture(other.current_texture) {
    other.font = nullptr;
    other.current_texture = nullptr;
}

TextRenderer& TextRenderer::operator=(TextRenderer&& other) noexcept {
    if (this != &other) {
        clearTexture();
        if (font) {
            TTF_CloseFont(font);
        }
        font = other.font;
        current_texture = other.current_texture;
        other.font = nullptr;
        other.current_texture = nullptr;
    }
    return *this;
}

void TextRenderer::clearTexture() {
    if (current_texture) {
        SDL_DestroyTexture(current_texture);
        current_texture = nullptr;
    }
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
    if (!font || text.empty()) return;
    
    clearTexture();
    
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "[CLIENT] Error creando surface de texto: " << TTF_GetError() << std::endl;
        return;
    }
    
    current_texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!current_texture) {
        std::cerr << "[CLIENT] Error creando textura de texto: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    
    SDL_Rect dst{ x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, current_texture, nullptr, &dst);
    SDL_FreeSurface(surface);
}

void TextRenderer::render_with_outline(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color, SDL_Color outline_color) {
    if (!font || text.empty()) return;
    
    render(renderer, text, x - 1, y, outline_color);
    render(renderer, text, x + 1, y, outline_color);
    render(renderer, text, x, y - 1, outline_color);
    render(renderer, text, x, y + 1, outline_color);
    render(renderer, text, x, y, color);
}
