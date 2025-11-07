#ifndef TALLER_TP_TEXT_RENDERER_H
#define TALLER_TP_TEXT_RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>

class TextRenderer {
    TTF_Font* font;
public:
    TextRenderer();
    explicit TextRenderer(const std::string& font_path, int ptsize);
    ~TextRenderer();

    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;

    TextRenderer(TextRenderer&&) noexcept;
    TextRenderer& operator=(TextRenderer&&) noexcept;

    bool load(const std::string& font_path, int ptsize);
    bool ok() const;
    void render(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color);
};

#endif // TALLER_TP_TEXT_RENDERER_H
