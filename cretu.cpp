#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <chrono>
#include <SDL2/SDL_ttf.h>

int bani()
{
    int chips = 50000;
    
    
    
    
    
    
    
    
    int multiplier[9] = {5, 10, 25, 50, 100, 500, 1000, 5000, 10000};
    int bet[5] = {250, 500, 1000, 2500, };
    return 0;
}














int main()
{
    SDL_Texture *pizdeBune[9] ={};
    SDL_Window *pizdePng;
    SDL_Init(SDL_INIT_VIDEO);
    pizdePng = SDL_CreateWindow("Pacanea Cu Desfranate", 0, 0, 1920, 1080, SDL_WINDOW_MAXIMIZED);
    SDL_Renderer *randat = NULL;
    randat = SDL_CreateRenderer(pizdePng, -1, SDL_RENDERER_ACCELERATED);
    IMG_Init(IMG_INIT_PNG);
    for(int i = 1; i <= 9 ; i++)
    {
      std::string folderPoze = "/home/theasspounder/vsc/pacaneav2/pozePacaneaDesfranata/";
      folderPoze = folderPoze + std::to_string(i) + ".png"; 
      SDL_Surface*lemonParty = IMG_Load(folderPoze.c_str());
      if(!lemonParty) std::cout << "pulaMea" << std::endl;
       pizdeBune[i - 1] = SDL_CreateTextureFromSurface(randat, lemonParty);
       SDL_FreeSurface(lemonParty);
    }
    SDL_Rect whores;
     TTF_Init();
     TTF_Font *fontChips;
     fontChips = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf", 15);
     SDL_Surface *textChips;
     textChips =  TTF_RenderText_Solid(fontChips, "Ai Atatea Belite: /n", {223, 194, 123});
     SDL_Texture *texChips;
     texChips = SDL_CreateTextureFromSurface(randat, textChips);
     

while(1 == 1)
{
    SDL_Event pimp;
    SDL_PollEvent(& pimp);
    if(pimp.type == SDL_QUIT)
{
    break;   
}
SDL_RenderClear(randat);
SDL_Rect whores;
whores.h = 200;
whores.w = 200;
whores.x = 760;
whores.y = 440;
SDL_RenderCopy(randat, pizdeBune[1], NULL, &whores);
SDL_RenderCopy(randat, texChips, NULL, &whores);
SDL_RenderPresent(randat);








}
    for(int i = 0; i <= 8; i++)
    {
    SDL_DestroyTexture(pizdeBune[i]);
    }

    SDL_DestroyRenderer(randat);
    SDL_DestroyWindow(pizdePng);   
    SDL_Quit(); 
    return 0;
}