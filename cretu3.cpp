#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <chrono>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <error.h>





bool apasareDeTaste ;
int belite;
//de introdus belitele


/*int bani()
{
    int chips = 50000;
    int multiplier[9] = {5, 10, 25, 50, 100, 500, 1000, 5000, 10000};
    int bet[5] = {250, 500, 1000, 2500, };
    
}*/

uint8_t clipRawOut[4 * 640 * 360];








int main()
{
    int belite = 50000;
    int bet = 2500;







    FILE * clipOutput;
    clipOutput = fopen("clipDesfranat/out.bin", "rb"); 
    if(!clipOutput) std::cout << "pulapizda" << std::endl;
    fread(clipRawOut, 1, 4 * 640 * 360, clipOutput);
    //rez clip 640x360
    fseek(clipOutput, 921600 * 5, SEEK_CUR);
    fread(clipRawOut, 1, 4 * 640 * 360, clipOutput);


    SDL_Texture *pizdeBune[9] ={};
    SDL_Window *pizdePng;
    SDL_Init(SDL_INIT_VIDEO);
    pizdePng = SDL_CreateWindow("Pacanea Cu Desfranate", 0, 0, 1920, 1080, SDL_WINDOW_SHOWN);
    SDL_Renderer *randat = NULL;
    randat = SDL_CreateRenderer(pizdePng, -1, SDL_RENDERER_ACCELERATED);
    IMG_Init(IMG_INIT_PNG);
    for(int i = 1; i <= 9 ; i++)
    {
      std::string folderPoze = "pozePacaneaDesfranata/";
      folderPoze = folderPoze + std::to_string(i) + ".png"; 
      SDL_Surface *lemonParty = IMG_Load(folderPoze.c_str());
      if(!lemonParty) std::cout << "pulaMea" << std::endl;
       pizdeBune[i - 1] = SDL_CreateTextureFromSurface(randat, lemonParty);
       SDL_FreeSurface(lemonParty);
    }
    SDL_Rect whores;
     TTF_Init();
     TTF_Font *fontChips;
     fontChips = TTF_OpenFont("/usr/share/fonts/hack/Hack-Bold.ttf", 15);
     SDL_Surface *textChips;
     std::string cateBeliteAm;
     cateBeliteAm = "Ai Atatea Belite: ";
     cateBeliteAm += std::to_string(belite);
     textChips =  TTF_RenderText_Solid(fontChips, cateBeliteAm.c_str(), {223, 194, 123});
     SDL_Texture *texChips;
     texChips = SDL_CreateTextureFromSurface(randat, textChips);
    
    SDL_Surface * framesClipInfo;
    framesClipInfo = SDL_CreateRGBSurfaceFrom(clipRawOut, 640, 360, 32, 2560, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_Texture * textureClip;
    textureClip = SDL_CreateTextureFromSurface(randat, framesClipInfo);
    







while(1 == 1)
{
    SDL_Event pimp;
    SDL_PollEvent(& pimp);
    if(pimp.type == SDL_QUIT)    
{
    break;   
}
    if(pimp.type == SDL_KEYDOWN)
    {
    apasareDeTaste = false;
                if(pimp.type == SDL_KEYDOWN)
                 {
                     apasareDeTaste = true;
                 }
    }
    
    
    if(pimp.type == SDL_KEYUP)
    {
        (char)pimp.key.keysym.sym;
            if(apasareDeTaste == true)
            {
                 std::cout << (char)pimp.key.keysym.sym << std::endl;
                apasareDeTaste = false;
                 apasareDeTaste != true;
                 if(pimp.key.keysym.sym == SDL_KeyCode::SDLK_RETURN)
                 {
                     std::cout << "penis" << std::endl;
                    SDL_DestroyTexture(texChips);
                     SDL_FreeSurface(textChips);
                     belite -= bet;
                    cateBeliteAm = "Ai Atatea Belite: ";
                    cateBeliteAm  += std::to_string(belite);
                    
                    textChips =  TTF_RenderText_Solid(fontChips, cateBeliteAm.c_str(), {223, 194, 123});
                    texChips = SDL_CreateTextureFromSurface(randat, textChips);

                 }
            }
    }
    

SDL_RenderClear(randat);
SDL_Rect whores0;
whores0.h = 200;
whores0.w = 200;
whores0.x = 420;
whores0.y = 550;
SDL_Rect whores1;
whores1.h = 200;
whores1.w = 200;
whores1.x = 940;
whores1.y = 550;
SDL_Rect whores2;
whores2.h = 200;
whores2.w = 200;
whores2.x = 1380;
whores2.y = 550;
SDL_Rect whoresText;
whoresText.h = 75;
whoresText.w = 500;
whoresText.x = 260;
whoresText.y = 240;
SDL_Rect randClip;
randClip.h = 1080;
randClip.w = 1920;
randClip.x = 0;
randClip.y = 0;

if(fread(clipRawOut, 1, 4 * 640 * 360, clipOutput) == 0)
{
    fseek(clipOutput, 921600 * 45, SEEK_SET);
}
SDL_DestroyTexture(textureClip);
SDL_FreeSurface(framesClipInfo);
framesClipInfo = SDL_CreateRGBSurfaceFrom(clipRawOut, 640, 360, 32, 2560, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
textureClip = SDL_CreateTextureFromSurface(randat, framesClipInfo);
SDL_RenderCopy(randat, textureClip, NULL, &randClip);
SDL_RenderCopy(randat, pizdeBune[2], NULL, &whores0);
SDL_RenderCopy(randat, pizdeBune[6], NULL, &whores1);
SDL_RenderCopy(randat, pizdeBune[8], NULL, &whores2);
SDL_RenderCopy(randat, texChips, NULL, &whoresText);
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